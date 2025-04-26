#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "DatabaseEntry.h"
#include <QByteArray>
#include <sqlite3.h>

struct DatabaseHandlerOptions
{
    std::string Description;
    std::string EncryptionAlgorithm;
    uint16_t EncryptionKeyLength;
    std::string KeyDerivationFunction;
    uint32_t KeyDerivationRounds;
    std::string KeySalt;
};

class DatabaseHandler
{
public:
    DatabaseHandler(const QString& filePath, const QByteArray& password, const DatabaseHandlerOptions* options = nullptr);
    ~DatabaseHandler();
    void createNewEntry(const DatabaseEntry& entry);

private:
    sqlite3* database = nullptr;
    DatabaseHandlerOptions dbOptions;
    QByteArray encryptionKey;

    void setNewDatabaseStructure();
    void createBasicInfoStructure();
    void createSettingsStructure();
    void createSecretsStructure();
    void fetchDatabaseBasicData();
};

#endif // DATABASEHANDLER_H
