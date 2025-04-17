#include "DatabaseHandler.h"
#include "../Crypto/Crypto.h"
#include "../Configs/Constants.h"

#include <stdexcept>
#include <sqlite3.h>
#include <QFile>

DatabaseHandler::DatabaseHandler(const QString& filePath, const QByteArray& password, const DatabaseHandlerOptions* options)
{
    this->password = password;
    bool dbIsNew = !QFile::exists(filePath);
    sqlite3_open_v2(filePath.toLatin1(), &this->database, (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), nullptr);
    if (dbIsNew)
        this->setNewDatabaseStructure(options);
}

DatabaseHandler::~DatabaseHandler()
{
    Crypto::wipeMemory(this->password.data(), this->password.length());
    if (this->database)
        sqlite3_close(this->database);
}

void DatabaseHandler::setNewDatabaseStructure(const DatabaseHandlerOptions* options)
{
    if (!options) throw std::runtime_error("Database options not informed");
    this->createBasicInfoStructure(options);
}

void DatabaseHandler::createBasicInfoStructure(const DatabaseHandlerOptions* options)
{
    std::string sql("CREATE TABLE basic_data ( entry_name varchar(100) not null primary key, value varchar(100) not null );");
    if (sqlite3_exec(this->database, sql.data(), nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    sqlite3_stmt* statement;
    sql = "INSERT INTO basic_data (entry_name, value) VALUES (?,?),(?,?),(?,?),(?,?);";
    if (sqlite3_prepare_v2(this->database, sql.data(), -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(this->database));

    sqlite3_bind_text(statement, 1, DATABASE_BASIC_DATA_DESCRIPTION, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, options->Description.toLatin1().data(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(statement, 3, DATABASE_BASIC_DATA_ENC_ALGORITHM, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, options->EncryptionAlgorithm.toUtf8().data(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(statement, 5, DATABASE_BASIC_DATA_ENC_KEY_LEN, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 6, options->EncryptionKeyLength);

    sqlite3_bind_text(statement, 7, DATABASE_BASIC_DATA_KEY_DERIVATION_FUNC, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 8, options->KeyDerivationFunction.toUtf8().data(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(this->database));
    if (sqlite3_finalize(statement) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(this->database));
}
