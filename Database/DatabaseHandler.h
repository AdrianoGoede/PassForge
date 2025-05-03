#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "DatabaseHandlerSettings.h"
#include "DatabaseEntry.h"
#include "CredentialEntry.h"
#include "ApiKeyEntry.h"
#include "CryptocurrencyEntry.h"
#include <QByteArray>
#include <sqlite3.h>
#include <QString>

struct DatabaseHandlerBasicData
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
    DatabaseHandler(const QString& filePath, const QByteArray& password, const DatabaseHandlerBasicData* basicData = nullptr);
    ~DatabaseHandler();
    void createNewEntry(const DatabaseEntry& entry);
    QVector<DatabaseEntry> getEntryHeaders() const;
    CredentialEntry getCredentialEntryBody(int entryId) const;
    CryptocurrencyEntry getCryptocurrencyEntryBody(int entryId) const;
    ApiKeyEntry getApiKeyEntryBody(int entryId) const;

private:
    sqlite3* database = nullptr;
    DatabaseHandlerBasicData dbBasicData;
    DatabaseHandlerSettings dbSettings;
    QByteArray encryptionKey;

    void setNewDatabaseStructure();
    void createBasicInfoStructure();
    void createSettingsStructure();
    void createSecretsStructure();
    void fetchDatabaseBasicData();
    void fetchDatabaseSettingsData();
    QByteArray getDatabaseEntryBody(int entryId) const;
    QString getCipherSetting(const QString& algorithm, size_t keyLength = 0);
};

#endif // DATABASEHANDLER_H
