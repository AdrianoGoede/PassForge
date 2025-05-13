#include "DatabaseHandler.h"
#include "../Crypto/Crypto.h"
#include "../Configs/Constants.h"
#include "../Configs/Configs.h"
#include "../Crypto/Crypto.h"
#include <QByteArray>
#include <stdexcept>
#include <sqlite3.h>
#include <QFile>

DatabaseHandler::DatabaseHandler(const QString& filePath, const QByteArray& password, const DatabaseHandlerBasicData* options)
{
    try {
        bool dbIsNew = !QFile::exists(filePath);
        if (sqlite3_open_v2(filePath.toLatin1(), &this->database, (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(this->database));

        if (dbIsNew) {
            if (!options) throw std::runtime_error("Database options not informed");
            this->dbBasicData = *options;
            this->dbBasicData.KeySalt = Crypto::generateSalt(this->dbBasicData.EncryptionKeyLength);
            this->dbBasicData.PasswordHash = Crypto::getHash(QByteArray(password).append(this->dbBasicData.KeySalt), DATABASE_DEFAULT_PASSWORD_ALGORITHM);
            this->setNewDatabaseStructure();
        }
        else
            this->fetchDatabaseBasicData();

        if (Crypto::getHash(QByteArray(password).append(this->dbBasicData.KeySalt), DATABASE_DEFAULT_PASSWORD_ALGORITHM) != this->dbBasicData.PasswordHash)
            throw std::runtime_error("Failed to unlock database");
        this->masterEncryptionKey = Crypto::deriveKey(password, this->dbBasicData.KeySalt, (this->dbBasicData.EncryptionKeyLength / 8), this->dbBasicData.KeyDerivationRounds, this->dbBasicData.KeyDerivationFunction);

        this->fetchDatabaseSettingsData();
        this->dbBasicData.EncryptionAlgorithm = this->getCipherSetting(this->dbBasicData.EncryptionAlgorithm, (this->masterEncryptionKey.length() * 8));
    }
    catch (...) {
        Crypto::wipeMemory(this->masterEncryptionKey.data(), this->masterEncryptionKey.length());
        throw;
    }
}

DatabaseHandler::~DatabaseHandler()
{
    Crypto::wipeMemory(this->masterEncryptionKey.data(), this->masterEncryptionKey.length());
    if (this->database)
        sqlite3_close(this->database);
}

void DatabaseHandler::saveDatabaseEntry(const DatabaseEntry& entry)
{
    QByteArray randomKey = Crypto::generateRandomKey(this->dbBasicData.EncryptionKeyLength / 8);
    QByteArray header = entry.getHeaderJson(), body = entry.getBodyJson();

    QByteArray encryptedRandomKey = Crypto::encrypt(randomKey, this->masterEncryptionKey, this->dbBasicData.EncryptionAlgorithm);
    QByteArray encryptedHeader = Crypto::encrypt(header, randomKey, this->dbBasicData.EncryptionAlgorithm);
    QByteArray encryptedBody = Crypto::encrypt(body, randomKey, this->dbBasicData.EncryptionAlgorithm);

    Crypto::wipeMemory(randomKey.data(), randomKey.length());
    Crypto::wipeMemory(header.data(), header.length());
    Crypto::wipeMemory(body.data(), body.length());

    const std::string sql = (entry.getEntryId() <= 0
                             ? "INSERT INTO secret_data (entry_key,header_data,body_data) VALUES (?,?,?)"
                             : "UPDATE secret_data SET entry_key = ?,header_data = ?,body_data = ? WHERE entry_id = ?");

    sqlite3_stmt* statement;
    try {
        if (sqlite3_prepare_v2(this->database, sql.data(), -1, &statement, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(this->database));

        sqlite3_bind_blob(statement, 1, encryptedRandomKey.data(), encryptedRandomKey.length(), SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 2, encryptedHeader.data(), encryptedHeader.length(), SQLITE_TRANSIENT);
        sqlite3_bind_blob(statement, 3, encryptedBody.data(), encryptedBody.length(), SQLITE_TRANSIENT);
        if (entry.getEntryId() > 0)
            sqlite3_bind_int(statement, 4, entry.getEntryId());

        if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }

    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
}

QVector<DatabaseEntry> DatabaseHandler::getEntryHeaders() const
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT entry_id,entry_key,header_data FROM secret_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    QVector<DatabaseEntry> results;
    try {
        while (sqlite3_step(statement) == SQLITE_ROW) {
            int entryId = sqlite3_column_int(statement, 0);
            QByteArray entryKey = Crypto::decrypt(
                QByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
                this->masterEncryptionKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            QByteArray header = Crypto::decrypt(
                QByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 2)), sqlite3_column_bytes(statement, 2)),
                entryKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            results.push_back(DatabaseEntry(header, entryId));
            Crypto::wipeMemory(header.data(), header.length());
            Crypto::wipeMemory(entryKey.data(), entryKey.length());
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
    QByteArray body = this->getDatabaseEntryBody(entry.getEntryId());
    CredentialEntry result(entry, body);
    result.setEntryId(entry.getEntryId());
    Crypto::wipeMemory(body.data(), body.length());
    return result;
}

CryptocurrencyEntry DatabaseHandler::getCryptocurrencyEntry(const DatabaseEntry& entry) const
{
    QByteArray body = this->getDatabaseEntryBody(entry.getEntryId());
    CryptocurrencyEntry result(entry, body);
    result.setEntryId(entry.getEntryId());
    Crypto::wipeMemory(body.data(), body.length());
    return result;
}

ApiKeyEntry DatabaseHandler::getApiKeyEntry(const DatabaseEntry& entry) const
{
    QByteArray body = this->getDatabaseEntryBody(entry.getEntryId());
    ApiKeyEntry result(entry, body);
    result.setEntryId(entry.getEntryId());
    Crypto::wipeMemory(body.data(), body.length());
    return result;
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
    const char sql[] = "CREATE TABLE settings_data ( section_name VARCHAR(100) NOT NULL PRIMARY KEY, section_key BLOB NOT NULL, payload BLOB NOT NULL );";
    if (sqlite3_exec(this->database, sql, nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::createSecretsStructure()
{
    const char sql[] = "CREATE TABLE secret_data ( entry_id INTEGER NOT NULL PRIMARY KEY, entry_key BLOB NOT NULL, header_data BLOB NOT NULL, body_data BLOB NOT NULL );";
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
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        QByteArray value(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1));
        if (!name || value.isEmpty()) continue;

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

void DatabaseHandler::fetchDatabaseSettingsData()
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT section_name,section_key,payload FROM settings_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    QByteArray generalData, securityData;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        QByteArray sectionKey = Crypto::decrypt(
            QByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
            this->masterEncryptionKey,
            this->dbBasicData.EncryptionAlgorithm
        );
        QByteArray value = Crypto::decrypt(
            QByteArray(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)), sqlite3_column_bytes(statement, 2)),
            sectionKey,
            this->dbBasicData.EncryptionAlgorithm
        );

        if (name && !value.isEmpty()) {
            if (std::strcmp(name, DATABASE_SETTINGS_DATA_GENERAL) == 0)
                generalData = value;
            else if (std::strcmp(name, DATABASE_SETTINGS_DATA_SECURITY) == 0)
                securityData = value;
        }

        Crypto::wipeMemory(sectionKey.data(), sectionKey.length());
        Crypto::wipeMemory(value.data(), value.length());
    }
    sqlite3_finalize(statement);

    this->dbSettings = DatabaseHandlerSettings(generalData, securityData);
    Crypto::wipeMemory(generalData.data(), generalData.length());
    Crypto::wipeMemory(securityData.data(), securityData.length());
}

QByteArray DatabaseHandler::getDatabaseEntryBody(int entryId) const
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT entry_key,body_data FROM secret_data WHERE entry_id = ?;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
    sqlite3_bind_int(statement, 1, entryId);

    try {
        if (sqlite3_step(statement) != SQLITE_ROW)
            throw std::runtime_error(sqlite3_errmsg(this->database));

        QByteArray entryKey = Crypto::decrypt(
            QByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 0)), sqlite3_column_bytes(statement, 0)),
            this->masterEncryptionKey,
            this->dbBasicData.EncryptionAlgorithm
        );
        QByteArray body = Crypto::decrypt(
            QByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
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

QString DatabaseHandler::getCipherSetting(const QString& algorithm, size_t keyLength)
{
    if (algorithm.trimmed().compare("AES", Qt::CaseSensitivity::CaseInsensitive) == 0)      return QString(CIPHER_SETTINGS_AES).arg(keyLength);
    if (algorithm.trimmed().compare("Serpent", Qt::CaseSensitivity::CaseInsensitive) == 0)  return QString(CIPHER_SETTINGS_SERPENT).arg(keyLength);
    if (algorithm.trimmed().compare("Twofish", Qt::CaseSensitivity::CaseInsensitive) == 0)  return QString(CIPHER_SETTINGS_TWOFISH).arg(keyLength);
    if (algorithm.trimmed().compare("Camellia", Qt::CaseSensitivity::CaseInsensitive) == 0) return QString(CIPHER_SETTINGS_CAMELLIA).arg(keyLength);
    if (algorithm.trimmed().compare("ChaCha20", Qt::CaseSensitivity::CaseInsensitive) == 0) return QString(CIPHER_SETTINGS_CHACHA20).arg(keyLength);
    throw std::runtime_error("Algorithm not supported");
}
