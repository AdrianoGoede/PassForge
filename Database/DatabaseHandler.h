#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "DatabaseHandlerSettings.h"
#include "../Crypto/SecureQByteArray.h"
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
    QByteArray PasswordHash;
};

class DatabaseHandler
{
public:
    DatabaseHandler(const QString& filePath, const SecureQByteArray& password, const DatabaseHandlerBasicData* basicData = nullptr);
    ~DatabaseHandler();
    void saveDatabaseEntry(const DatabaseEntry& entry);
    QVector<DatabaseEntry> getEntryHeaders() const;
    CredentialEntry getCredentialEntry(const DatabaseEntry& entry) const;
    CryptocurrencyEntry getCryptocurrencyEntry(const DatabaseEntry& entry) const;
    ApiKeyEntry getApiKeyEntry(const DatabaseEntry& entry) const;

private:
    sqlite3* database = nullptr;
    DatabaseHandlerBasicData dbBasicData;
    DatabaseHandlerSettings dbSettings;
    SecureQByteArray masterEncryptionKey;

    void setNewDatabaseStructure();
    void createBasicInfoStructure();
    void createSettingsStructure();
    void createSecretsStructure();
    void fetchDatabaseBasicData();
    void fetchDatabaseSettingsData();
    SecureQByteArray getDatabaseEntryBody(int entryId) const;
    QString getCipherSetting(const QString& algorithm, size_t keyLength = 0);
};

#endif // DATABASEHANDLER_H
