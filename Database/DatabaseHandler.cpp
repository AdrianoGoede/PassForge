#include "DatabaseHandler.h"
#include "../Crypto/Crypto.h"
#include "../Configs/Constants.h"
#include "../Crypto/Crypto.h"
#include <stdexcept>
#include <sqlite3.h>
#include <QFile>

DatabaseHandler::DatabaseHandler(const QString& filePath, const QByteArray& password, const DatabaseHandlerOptions* options)
{
    bool dbIsNew = !QFile::exists(filePath);
    if (sqlite3_open_v2(filePath.toLatin1(), &this->database, (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    if (dbIsNew) {
        if (!options) throw std::runtime_error("Database options not informed");
        this->dbOptions = *options;
        this->dbOptions.KeySalt = Crypto::generateSalt(this->dbOptions.EncryptionKeyLength);
        this->setNewDatabaseStructure();
    }
    else
        this->fetchDatabaseBasicData();

    if (options->KeyDerivationFunction == "PBKDF2")
        this->encryptionKey = Crypto::deriveKeyPbkdf2(password.data(), this->dbOptions.KeySalt, this->dbOptions.EncryptionKeyLength, this->dbOptions.KeyDerivationRounds);
    else if (options->KeyDerivationFunction == "Scrypt")
        this->encryptionKey = Crypto::deriveKeyScrypt(password.data(), password.length(), this->dbOptions.KeySalt, this->dbOptions.EncryptionKeyLength, this->dbOptions.KeyDerivationRounds);
    else if (options->KeyDerivationFunction == "Argon2id") {
        // TO DO!
    }
    else { throw std::runtime_error("Key derivation function not supported"); }
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

    const char sql[] = "INSERT INTO secret_data (header_data,body_data) VALUES (?,?)";
    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(this->database, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    sqlite3_bind_text(statement, 1, header.data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, body.data(), -1, SQLITE_TRANSIENT);

    Crypto::wipeMemory(header.data(), header.length());
    Crypto::wipeMemory(body.data(), body.length());

    if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
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
    sql = "INSERT INTO basic_data (entry_name, value) VALUES (?,?),(?,?),(?,?),(?,?),(?,?);";
    if (sqlite3_prepare_v2(this->database, sql.data(), -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    sqlite3_bind_text(statement, 1, DATABASE_BASIC_DATA_DESCRIPTION, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, this->dbOptions.Description.data(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(statement, 3, DATABASE_BASIC_DATA_ENC_ALGORITHM, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, this->dbOptions.EncryptionAlgorithm.data(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(statement, 5, DATABASE_BASIC_DATA_ENC_KEY_LEN, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 6, this->dbOptions.EncryptionKeyLength);

    sqlite3_bind_text(statement, 7, DATABASE_BASIC_DATA_KEY_DERIVATION_FUNC, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 8, this->dbOptions.KeyDerivationFunction.data(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(statement, 9, DATABASE_BASIC_DATA_KEY_DERIVATION_SALT, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 10, this->dbOptions.KeySalt.data(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
}

void DatabaseHandler::createSettingsStructure()
{
    const char sql[] = "CREATE TABLE database_settings ( entry_name VARCHAR(100) NOT NULL PRIMARY KEY, value BLOB NOT NULL );";
    if (sqlite3_exec(this->database, sql, nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    // TO DO!
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
            this->dbOptions.Description = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_ENC_ALGORITHM) == 0)
            this->dbOptions.EncryptionAlgorithm = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_ENC_KEY_LEN) == 0)
            this->dbOptions.EncryptionKeyLength = std::stoi(value);
        else if (std::strcmp(name, DATABASE_BASIC_DATA_KEY_DERIVATION_FUNC) == 0)
            this->dbOptions.KeyDerivationFunction = value;
        else if (std::strcmp(name, DATABASE_BASIC_DATA_KEY_DERIVATION_SALT) == 0)
            this->dbOptions.KeySalt = value;
    }
    sqlite3_finalize(statement);
}
