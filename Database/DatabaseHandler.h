#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

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
    void deleteDatabaseEntry(int entryId) const;
    QVector<SecureQByteArray> getEntryPaths() const;
    QVector<DatabaseEntry> getEntryHeaders(const SecureQByteArray& path) const;
    CredentialEntry getCredentialEntry(const DatabaseEntry& entry) const;
    CryptocurrencyEntry getCryptocurrencyEntry(const DatabaseEntry& entry) const;
    ApiKeyEntry getApiKeyEntry(const DatabaseEntry& entry) const;
    SecureQByteArray getDatabaseSetting(const QString& settingName);

private:
    sqlite3* database = nullptr;
    DatabaseHandlerBasicData dbBasicData;
    SecureQByteArray masterEncryptionKey;

    void setNewDatabaseStructure();
    void createBasicInfoStructure();
    void createSettingsStructure();
    void createSecretsStructure();
    void fetchDatabaseBasicData();
    SecureQByteArray getDatabaseEntryBody(int entryId) const;
    QString getCipherSetting(const QString& algorithm, size_t keyLength = 0);
};

#endif // DATABASEHANDLER_H
