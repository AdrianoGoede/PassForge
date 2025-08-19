#include "DatabaseHandler.h"
#include "../Crypto/Crypto.h"
#include "../Configs/Constants.h"
#include "../Configs/Configs.h"
#include <QByteArray>
#include <stdexcept>
#include <sqlite3.h>
#include <QFile>
#include <QSet>
#include <QMap>

DatabaseHandler::DatabaseHandler(const QString& filePath, const SecureQByteArray& password, const DatabaseHandlerBasicData* options)
{
    bool dbIsNew = !QFile::exists(filePath);
    if (sqlite3_open_v2(filePath.toLatin1(), &this->database, (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    if (dbIsNew) {
        if (!options) throw std::runtime_error("Database options not informed");
        this->dbBasicData = DatabaseHandlerBasicData(*options);
        this->dbBasicData.EncryptionAlgorithm = this->getCipherSetting(this->dbBasicData.EncryptionAlgorithm, this->dbBasicData.EncryptionKeyLength);
        this->dbBasicData.KeySalt = Crypto::generateSalt(this->dbBasicData.EncryptionKeyLength);
        this->dbBasicData.PasswordHash = Crypto::getHash(SecureQByteArray(password) + this->dbBasicData.KeySalt, DATABASE_DEFAULT_PASSWORD_ALGORITHM);
        this->masterEncryptionKey = Crypto::deriveKey(password, this->dbBasicData.KeySalt.data(), (this->dbBasicData.EncryptionKeyLength / 8), this->dbBasicData.KeyDerivationRounds, this->dbBasicData.KeyDerivationFunction);
        this->setNewDatabaseStructure();
    }
    else
        this->fetchDatabaseBasicData();

    if (Crypto::getHash(SecureQByteArray(password + this->dbBasicData.KeySalt), DATABASE_DEFAULT_PASSWORD_ALGORITHM) != this->dbBasicData.PasswordHash)
        throw std::runtime_error("Failed to unlock database");
    this->masterEncryptionKey = Crypto::deriveKey(password, this->dbBasicData.KeySalt.data(), (this->dbBasicData.EncryptionKeyLength / 8), this->dbBasicData.KeyDerivationRounds, this->dbBasicData.KeyDerivationFunction);
}

DatabaseHandler::~DatabaseHandler()
{
    if (this->database)
        sqlite3_close(this->database);
}

void DatabaseHandler::saveDatabaseEntry(const DatabaseEntry& entry)
{
    SecureQByteArray randomKey = Crypto::generateRandomKey(this->dbBasicData.EncryptionKeyLength / 8);
    SecureQByteArray path = Crypto::encrypt(entry.getPath(), randomKey, this->dbBasicData.EncryptionAlgorithm);
    SecureQByteArray header = Crypto::encrypt(entry.getHeaderJson(), randomKey, this->dbBasicData.EncryptionAlgorithm);
    SecureQByteArray body = Crypto::encrypt(entry.getBodyJson(), randomKey, this->dbBasicData.EncryptionAlgorithm);
    randomKey = Crypto::encrypt(randomKey, this->masterEncryptionKey, this->dbBasicData.EncryptionAlgorithm);

    const std::string sql = (entry.getEntryId() <= 0
                             ? "INSERT INTO secret_data (entry_key,path,header_data,body_data) VALUES (?,?,?,?)"
                             : "UPDATE secret_data SET entry_key = ?, path = ?,header_data = ?,body_data = ? WHERE entry_id = ?");

    sqlite3_stmt* statement;
    try {
        if (sqlite3_prepare_v2(this->database, sql.data(), -1, &statement, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(this->database));

        sqlite3_bind_blob(statement, 1, randomKey.data(), randomKey.length(), SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 2, path.data(), path.length(), SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 3, header.data(), header.length(), SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 4, body.data(), body.length(), SQLITE_TRANSIENT);
        if (entry.getEntryId() > 0)
            sqlite3_bind_int(statement, 5, entry.getEntryId());

        if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }

    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::deleteDatabaseEntry(int entryId) const
{
    sqlite3_stmt* statement;
    const char sql[] = "DELETE FROM secret_data WHERE entry_id = ?;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
    sqlite3_bind_int(statement, 1, entryId);

    try {
        if (sqlite3_step(statement) != SQLITE_DONE)
            throw std::runtime_error(sqlite3_errmsg(this->database));
        sqlite3_finalize(statement);
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }
}

QVector<SecureQByteArray> DatabaseHandler::getEntryPaths() const
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT entry_key,path FROM secret_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    QSet<SecureQByteArray> results;
    try {
        while (sqlite3_step(statement) == SQLITE_ROW) {
            SecureQByteArray entryKey = Crypto::decrypt(
                SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 0)), sqlite3_column_bytes(statement, 0)),
                this->masterEncryptionKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            results.insert(Crypto::decrypt(
                SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
                entryKey,
                this->dbBasicData.EncryptionAlgorithm
            ));
        }
        sqlite3_finalize(statement);
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }

    return QVector<SecureQByteArray>(results.cbegin(), results.cend());
}

QVector<DatabaseEntry> DatabaseHandler::getEntryHeaders(const SecureQByteArray& path) const
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT entry_id,entry_key,path,header_data FROM secret_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    QVector<DatabaseEntry> results;
    try {
        while (sqlite3_step(statement) == SQLITE_ROW) {
            int entryId = sqlite3_column_int(statement, 0);
            SecureQByteArray entryKey = Crypto::decrypt(
                SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
                this->masterEncryptionKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            SecureQByteArray entryPath = Crypto::decrypt(
                SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 2)), sqlite3_column_bytes(statement, 2)),
                entryKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            if (entryPath != path) continue;

            SecureQByteArray header = Crypto::decrypt(
                SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 3)), sqlite3_column_bytes(statement, 3)),
                entryKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            results.push_back(DatabaseEntry(header, entryId));
        }
        sqlite3_finalize(statement);
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }

    return results;
}

CredentialEntry DatabaseHandler::getCredentialEntry(const DatabaseEntry& entry) const
{
    SecureQByteArray body = this->getDatabaseEntryBody(entry.getEntryId());
    CredentialEntry result(entry, body);
    return result;
}

CryptocurrencyEntry DatabaseHandler::getCryptocurrencyEntry(const DatabaseEntry& entry) const
{
    SecureQByteArray body = this->getDatabaseEntryBody(entry.getEntryId());
    CryptocurrencyEntry result(entry, body);
    return result;
}

ApiKeyEntry DatabaseHandler::getApiKeyEntry(const DatabaseEntry& entry) const
{
    SecureQByteArray body = this->getDatabaseEntryBody(entry.getEntryId());
    ApiKeyEntry result(entry, body);
    return result;
}

SecureQByteArray DatabaseHandler::getDatabaseSetting(const QString& settingName) const
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT key,value FROM settings_data WHERE name = ?;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
    sqlite3_bind_text(statement, 1, settingName.toUtf8(), -1, SQLITE_TRANSIENT);

    try {
        SecureQByteArray result;

        if (sqlite3_step(statement) == SQLITE_ROW) {
            SecureQByteArray entryKey = Crypto::decrypt(
                SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 0)), sqlite3_column_bytes(statement, 0)),
                this->masterEncryptionKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            result = Crypto::decrypt(
                SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
                entryKey,
                this->dbBasicData.EncryptionAlgorithm
            );
        }

        sqlite3_finalize(statement);
        return result;
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }
}

void DatabaseHandler::saveDatabaseSetting(const QString& settingName, const SecureQByteArray& value) const
{
    if (sqlite3_exec(this->database, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    sqlite3_stmt* statement;
    const char sql[] = "INSERT OR REPLACE INTO settings_data (name, key, value) VALUES (?,?,?);";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    try {
        SecureQByteArray key = Crypto::generateRandomKey(this->dbBasicData.EncryptionKeyLength / 8);
        SecureQByteArray encryptedValue = Crypto::encrypt(value, key, this->dbBasicData.EncryptionAlgorithm);
        key = Crypto::encrypt(key, this->masterEncryptionKey, this->dbBasicData.EncryptionAlgorithm);

        sqlite3_bind_text(statement, 1, settingName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 2, key.data(), key.size(), SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 3, encryptedValue.data(), encryptedValue.size(), SQLITE_TRANSIENT);

        if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
        if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));

        this->processSettingChange(settingName, value);

        if (sqlite3_exec(this->database, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(this->database));
    }
    catch (...) {
        sqlite3_finalize(statement);
        sqlite3_exec(this->database, "ROLLBACK;", nullptr, nullptr, nullptr);
        throw;
    }
}

void DatabaseHandler::setNewDatabaseStructure()
{
    this->createBasicInfoStructure();
    this->createSettingsStructure();
    this->createSecretsStructure();
}

void DatabaseHandler::createBasicInfoStructure()
{
    std::string sql("CREATE TABLE basic_data ( entry_name VARCHAR(100) NOT NULL PRIMARY KEY, value BLOB NOT NULL );");
    if (sqlite3_exec(this->database, sql.data(), nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    sqlite3_stmt* statement;
    try {
        sql = "INSERT INTO basic_data (entry_name, value) VALUES (?,?),(?,?),(?,?),(?,?),(?,?),(?,?),(?,?);";
        if (sqlite3_prepare_v2(this->database, sql.data(), -1, &statement, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(this->database));

        sqlite3_bind_text(statement, 1, DATABASE_BASIC_DATA_DESCRIPTION, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(statement, 2, this->dbBasicData.Description.toUtf8().data(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(statement, 3, DATABASE_BASIC_DATA_ENC_ALGORITHM, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(statement, 4, this->dbBasicData.EncryptionAlgorithm.toUtf8().data(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(statement, 5, DATABASE_BASIC_DATA_ENC_KEY_LEN, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(statement, 6, this->dbBasicData.EncryptionKeyLength);

        sqlite3_bind_text(statement, 7, DATABASE_BASIC_DATA_KEY_DERIVATION_FUNC, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(statement, 8, this->dbBasicData.KeyDerivationFunction.toUtf8().data(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(statement, 9, DATABASE_BASIC_DATA_KEY_DERIVATION_ROUNDS, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(statement, 10, this->dbBasicData.KeyDerivationRounds);

        sqlite3_bind_text(statement, 11, DATABASE_BASIC_DATA_KEY_DERIVATION_SALT, -1, SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 12, this->dbBasicData.KeySalt.data(), this->dbBasicData.KeySalt.length(), SQLITE_TRANSIENT);

        sqlite3_bind_text(statement, 13, DATABASE_BASIC_DATA_PASSWORD_HASH, -1, SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 14, this->dbBasicData.PasswordHash.data(), this->dbBasicData.PasswordHash.length(), SQLITE_TRANSIENT);

        if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }

    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::createSettingsStructure()
{
    std::string sql = "CREATE TABLE settings_data ( name TEXT NOT NULL PRIMARY KEY, key BLOB NOT NULL, value BLOB NOT NULL );";
    if (sqlite3_exec(this->database, sql.data(), nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    QMap<QString, QString> defaultSettings = DEFAULT_DATABASE_SETTINGS;
    for (const QString& setting : defaultSettings.keys())
        this->saveDatabaseSetting(setting, defaultSettings.value(setting, QString()).toUtf8().constData());
}

void DatabaseHandler::createSecretsStructure()
{
    const char sql[] = "CREATE TABLE secret_data ( entry_id INTEGER NOT NULL PRIMARY KEY, entry_key BLOB NOT NULL, path BLOB NOT NULL, header_data BLOB NOT NULL, body_data BLOB NOT NULL );";
    if (sqlite3_exec(this->database, sql, nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::fetchDatabaseBasicData()
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT entry_name,value FROM basic_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    while (sqlite3_step(statement) == SQLITE_ROW) {
        SecureQByteArray name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        SecureQByteArray value(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1));
        if (name.isEmpty() || value.isEmpty()) continue;

        if (std::strcmp(name, DATABASE_BASIC_DATA_DESCRIPTION) == 0)
            this->dbBasicData.Description = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_ENC_ALGORITHM) == 0)
            this->dbBasicData.EncryptionAlgorithm = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_ENC_KEY_LEN) == 0)
            this->dbBasicData.EncryptionKeyLength = std::stoi(value.data());
        else if (std::strcmp(name, DATABASE_BASIC_DATA_KEY_DERIVATION_FUNC) == 0)
            this->dbBasicData.KeyDerivationFunction = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_KEY_DERIVATION_ROUNDS) == 0)
            this->dbBasicData.KeyDerivationRounds = std::stoi(value.data());
        else if (std::strcmp(name, DATABASE_BASIC_DATA_KEY_DERIVATION_SALT) == 0)
            this->dbBasicData.KeySalt = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_PASSWORD_HASH) == 0)
            this->dbBasicData.PasswordHash = value;
    }
    sqlite3_finalize(statement);
}

void DatabaseHandler::processSettingChange(const QString& setting, const SecureQByteArray& value) const
{
    if (setting == DB_SETTINGS_NAME_RANDOM_PADDING_INTERVAL) {
        sqlite3_stmt* selectStatement;
        sqlite3_stmt* updateStatement;

        try {
            std::string sql = "SELECT entry_id,entry_key,path,header_data,body_data FROM secret_data;";
            if (sqlite3_prepare_v2(this->database, sql.c_str(), -1, &selectStatement, nullptr) != SQLITE_OK)
                throw std::runtime_error(sqlite3_errmsg(this->database));
            sql = "UPDATE secret_data SET entry_key = ?, path = ?, header_data = ?, body_data = ? WHERE entry_id = ?;";
            if (sqlite3_prepare_v2(this->database, sql.c_str(), -1, &updateStatement, nullptr) != SQLITE_OK)
                throw std::runtime_error(sqlite3_errmsg(this->database));

            bool minOk, maxOk;
            QVector<SecureQByteArray> valueParts = value.secureSplit('-');
            if (valueParts.size() != 2) throw std::runtime_error("Invalid padding interval format. Expected 'min-max'");
            int min = QString(valueParts.at(0)).toInt(&minOk);
            int max = QString(valueParts.at(1)).toInt(&maxOk);
            if (!minOk || !maxOk || max < min) throw std::runtime_error("Invalid padding interval");

            while (sqlite3_step(selectStatement) == SQLITE_ROW) {
                int entryId = sqlite3_column_int(selectStatement, 0);
                SecureQByteArray entryKey;
                SecureQByteArray path;
                SecureQByteArray headerData;
                SecureQByteArray bodyData;

                this->getEntryValues(selectStatement, entryKey, path, headerData, bodyData);
                this->prepareEntryValues(min, max, entryKey, path, headerData, bodyData);

                sqlite3_bind_blob(updateStatement, 1, entryKey.constData(), entryKey.size(), SQLITE_TRANSIENT);
                sqlite3_bind_blob(updateStatement, 2, path.constData(), path.size(), SQLITE_TRANSIENT);
                sqlite3_bind_blob(updateStatement, 3, headerData.constData(), headerData.size(), SQLITE_TRANSIENT);
                sqlite3_bind_blob(updateStatement, 4, bodyData.constData(), bodyData.size(), SQLITE_TRANSIENT);
                sqlite3_bind_int(updateStatement, 5, entryId);

                if (sqlite3_step(updateStatement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
                sqlite3_reset(updateStatement);
            }

            sqlite3_finalize(selectStatement);
            sqlite3_finalize(updateStatement);
        }
        catch (...) {
            sqlite3_finalize(selectStatement);
            sqlite3_finalize(updateStatement);
            throw;
        }
    }
}

void DatabaseHandler::getEntryValues(sqlite3_stmt* statement, SecureQByteArray& entryKey, SecureQByteArray& path, SecureQByteArray& headerData, SecureQByteArray& bodyData) const
{
    if (!statement) return;
    entryKey = Crypto::decrypt(
        SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
        this->masterEncryptionKey,
        this->dbBasicData.EncryptionAlgorithm
    );
    path = Crypto::decrypt(
        SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 2)), sqlite3_column_bytes(statement, 2)),
        entryKey,
        this->dbBasicData.EncryptionAlgorithm
    ).secureSplit(DB_SETTINGS_PADDING_SEPARATOR).at(0);
    headerData = Crypto::decrypt(
        SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 3)), sqlite3_column_bytes(statement, 3)),
        entryKey,
        this->dbBasicData.EncryptionAlgorithm
    ).secureSplit(DB_SETTINGS_PADDING_SEPARATOR).at(0);
    bodyData = Crypto::decrypt(
        SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 4)), sqlite3_column_bytes(statement, 4)),
        entryKey,
        this->dbBasicData.EncryptionAlgorithm
    ).secureSplit(DB_SETTINGS_PADDING_SEPARATOR).at(0);
}

void DatabaseHandler::prepareEntryValues(int min, int max, SecureQByteArray& entryKey, SecureQByteArray& path, SecureQByteArray& headerData, SecureQByteArray& bodyData) const
{
    entryKey = Crypto::generateRandomKey(this->dbBasicData.EncryptionKeyLength / 8);
    path = Crypto::encrypt(
        SecureQByteArray(path + DB_SETTINGS_PADDING_SEPARATOR + Crypto::generateRandomBlob(min, max)),
        entryKey,
        this->dbBasicData.EncryptionAlgorithm
    );
    headerData = Crypto::encrypt(
        SecureQByteArray(headerData + DB_SETTINGS_PADDING_SEPARATOR + Crypto::generateRandomBlob(min, max)),
        entryKey,
        this->dbBasicData.EncryptionAlgorithm
    );
    bodyData = Crypto::encrypt(
        SecureQByteArray(bodyData + DB_SETTINGS_PADDING_SEPARATOR + Crypto::generateRandomBlob(min, max)),
        entryKey,
        this->dbBasicData.EncryptionAlgorithm
    );
    entryKey = Crypto::encrypt(entryKey, this->masterEncryptionKey, this->dbBasicData.EncryptionAlgorithm);
}

SecureQByteArray DatabaseHandler::getDatabaseEntryBody(int entryId) const
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT entry_key,body_data FROM secret_data WHERE entry_id = ?;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
    sqlite3_bind_int(statement, 1, entryId);

    try {
        if (sqlite3_step(statement) != SQLITE_ROW)
            throw std::runtime_error(sqlite3_errmsg(this->database));

        SecureQByteArray entryKey = Crypto::decrypt(
            SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 0)), sqlite3_column_bytes(statement, 0)),
            this->masterEncryptionKey,
            this->dbBasicData.EncryptionAlgorithm
        );
        SecureQByteArray body = Crypto::decrypt(
            SecureQByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
            entryKey,
            this->dbBasicData.EncryptionAlgorithm
        );
        return body;
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }
}

QString DatabaseHandler::getCipherSetting(const QString& algorithm, size_t keyLength) const
{
    if (algorithm.trimmed().compare("AES", Qt::CaseSensitivity::CaseInsensitive) == 0)      return QString(CIPHER_SETTINGS_AES).arg(keyLength);
    if (algorithm.trimmed().compare("Serpent", Qt::CaseSensitivity::CaseInsensitive) == 0)  return QString(CIPHER_SETTINGS_SERPENT);
    if (algorithm.trimmed().compare("Twofish", Qt::CaseSensitivity::CaseInsensitive) == 0)  return QString(CIPHER_SETTINGS_TWOFISH);
    if (algorithm.trimmed().compare("Camellia", Qt::CaseSensitivity::CaseInsensitive) == 0) return QString(CIPHER_SETTINGS_CAMELLIA).arg(keyLength);
    if (algorithm.trimmed().compare("ChaCha20", Qt::CaseSensitivity::CaseInsensitive) == 0) return QString(CIPHER_SETTINGS_CHACHA20);
    throw std::runtime_error("Algorithm not supported");
}
