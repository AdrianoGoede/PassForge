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
    bool dbIsNew = !QFile::exists(filePath);
    if (sqlite3_open_v2(filePath.toLatin1(), &this->database, (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    if (dbIsNew) {
        if (!options) throw std::runtime_error("Database options not informed");
        this->dbBasicData = *options;
        this->dbBasicData.KeySalt = Crypto::generateSalt(this->dbBasicData.EncryptionKeyLength);
        this->setNewDatabaseStructure();
    }
    else {
        this->fetchDatabaseBasicData();
        this->fetchDatabaseSettingsData();
    }

    if (options->KeyDerivationFunction == "PBKDF2")
        this->encryptionKey = Crypto::deriveKeyPbkdf2(password, this->dbBasicData.KeySalt, this->dbBasicData.EncryptionKeyLength, this->dbBasicData.KeyDerivationRounds);
    else if (options->KeyDerivationFunction == "Scrypt")
        this->encryptionKey = Crypto::deriveKeyScrypt(password, this->dbBasicData.KeySalt, this->dbBasicData.EncryptionKeyLength, this->dbBasicData.KeyDerivationRounds);
    else if (options->KeyDerivationFunction == "Argon2id")
        this->encryptionKey = Crypto::deriveKeyArgon2id(password, this->dbBasicData.KeySalt, this->dbBasicData.EncryptionKeyLength, this->dbBasicData.KeyDerivationRounds);
    else throw std::runtime_error("Key derivation function not supported");

    this->dbBasicData.EncryptionAlgorithm = this->getCipherSetting(this->dbBasicData.EncryptionAlgorithm, this->encryptionKey.length() * 8);
}

DatabaseHandler::~DatabaseHandler()
{
    Crypto::wipeMemory(this->encryptionKey.data(), this->encryptionKey.length());
    if (this->database)
        sqlite3_close(this->database);
}

void DatabaseHandler::createNewEntry(const DatabaseEntry& entry)
{
    QByteArray header{ entry.getHeaderJson() }, body{ entry.getBodyJson() };
    QByteArray encryptedHeader = Crypto::encrypt(header, this->encryptionKey, this->dbBasicData.EncryptionAlgorithm);
    QByteArray encryptedBody = Crypto::encrypt(body, this->encryptionKey, this->dbBasicData.EncryptionAlgorithm);
    Crypto::wipeMemory(header.data(), header.length());
    Crypto::wipeMemory(body.data(), body.length());

    const char sql[] = "INSERT INTO secret_data (header_data,body_data) VALUES (?,?)";
    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    sqlite3_bind_text(statement, 1, encryptedHeader.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, encryptedBody.data(), -1, SQLITE_TRANSIENT);

    Crypto::wipeMemory(header.data(), header.length());
    Crypto::wipeMemory(body.data(), body.length());

    if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
}

QVector<DatabaseEntry> DatabaseHandler::getEntryHeaders() const
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT entry_id,header_data FROM secret_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    QVector<DatabaseEntry> results;
    try {
        while (sqlite3_step(statement) == SQLITE_ROW) {
            int entryId = sqlite3_column_int(statement, 0);
            QByteArray header = Crypto::decrypt(
                QByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 1)), sqlite3_column_bytes(statement, 1)),
                this->encryptionKey,
                this->dbBasicData.EncryptionAlgorithm
            );
            results.push_back(DatabaseEntry(header, entryId));
            Crypto::wipeMemory(header.data(), header.length());
        }
        sqlite3_finalize(statement);
    }
    catch (...) {
        sqlite3_finalize(statement);
        throw;
    }

    return results;
}

CredentialEntry DatabaseHandler::getCredentialEntryBody(int entryId) const
{
    QByteArray body = this->getDatabaseEntryBody(entryId);
    CredentialEntry result(QByteArray(), body);
    Crypto::wipeMemory(body.data(), body.length());
    return result;
}

CryptocurrencyEntry DatabaseHandler::getCryptocurrencyEntryBody(int entryId) const
{
    QByteArray body = this->getDatabaseEntryBody(entryId);
    CryptocurrencyEntry result(QByteArray(), body);
    Crypto::wipeMemory(body.data(), body.length());
    return result;
}

ApiKeyEntry DatabaseHandler::getApiKeyEntryBody(int entryId) const
{
    QByteArray body = this->getDatabaseEntryBody(entryId);
    ApiKeyEntry result(QByteArray(), body);
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
    sql = "INSERT INTO basic_data (entry_name, value) VALUES (?,?),(?,?),(?,?),(?,?),(?,?),(?,?);";
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
    sqlite3_bind_blob(statement, 12, this->dbBasicData.KeySalt.data(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::createSettingsStructure()
{
    const char sql[] = "CREATE TABLE settings_data ( entry_name VARCHAR(100) NOT NULL PRIMARY KEY, value BLOB NOT NULL );";
    if (sqlite3_exec(this->database, sql, nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::createSecretsStructure()
{
    const char sql[] = "CREATE TABLE secret_data ( entry_id INTEGER NOT NULL PRIMARY KEY, header_data BLOB NOT NULL, body_data BLOB NOT NULL );";
    if (sqlite3_exec(this->database, sql, nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::fetchDatabaseBasicData()
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT * FROM basic_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    while (sqlite3_step(statement) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        const char* value = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        if (!name || !value) continue;

        if (std::strcmp(name, DATABASE_BASIC_DATA_DESCRIPTION) == 0)
            this->dbBasicData.Description = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_ENC_ALGORITHM) == 0)
            this->dbBasicData.EncryptionAlgorithm = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_ENC_KEY_LEN) == 0)
            this->dbBasicData.EncryptionKeyLength = std::stoi(value);
        else if (std::strcmp(name, DATABASE_BASIC_DATA_KEY_DERIVATION_FUNC) == 0)
            this->dbBasicData.KeyDerivationFunction = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_KEY_DERIVATION_SALT) == 0)
            this->dbBasicData.KeySalt = value;
    }
    sqlite3_finalize(statement);
}

void DatabaseHandler::fetchDatabaseSettingsData()
{
    sqlite3_stmt* statement;
    const char sql[] = "SELECT * FROM settings_data;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    QByteArray generalData, securityData;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        QByteArray value = Crypto::decrypt(
            QByteArray(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1), sqlite3_column_bytes(statement, 1))),
            this->encryptionKey,
            this->dbBasicData.EncryptionAlgorithm
        );
        if (!name || value.isEmpty()) continue;

        if (std::strcmp(name, DATABASE_SETTINGS_DATA_GENERAL) == 0)
            generalData = value;
        else if (std::strcmp(name, DATABASE_SETTINGS_DATA_SECURITY) == 0)
            securityData = value;

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
    const char sql[] = "SELECT body_data FROM secret_data WHERE entry_id = ?;";
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));
    sqlite3_bind_int(statement, 1, entryId);

    try {
        if (sqlite3_step(statement) != SQLITE_ROW)
            throw std::runtime_error(sqlite3_errmsg(this->database));

        QByteArray body = Crypto::decrypt(
            QByteArray(reinterpret_cast<const char*>(sqlite3_column_blob(statement, 0)), sqlite3_column_bytes(statement, 0)),
            this->encryptionKey,
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
    if (algorithm.trimmed().compare("Serpent", Qt::CaseSensitivity::CaseInsensitive) == 0)  return CIPHER_SETTINGS_SERPENT;
    if (algorithm.trimmed().compare("Twofish", Qt::CaseSensitivity::CaseInsensitive) == 0)  return CIPHER_SETTINGS_TWOFISH;
    if (algorithm.trimmed().compare("Camellia", Qt::CaseSensitivity::CaseInsensitive) == 0) return CIPHER_SETTINGS_CAMELLIA;
    if (algorithm.trimmed().compare("ChaCha20", Qt::CaseSensitivity::CaseInsensitive) == 0) return CIPHER_SETTINGS_CHACHA20;
    throw std::runtime_error("Algorithm not supported");
}
