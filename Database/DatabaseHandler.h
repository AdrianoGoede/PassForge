#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "DatabaseEntry.h"
#include <QByteArray>
#include <sqlite3.h>
#include <QString>

struct DatabaseHandlerOptions
{
    QString Description;
    QString EncryptionAlgorithm;
    uint16_t EncryptionKeyLength;
    QString KeyDerivationFunction;
    uint32_t KeyDerivationRounds;
    QByteArray KeySalt;
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
