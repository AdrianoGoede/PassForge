#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QString>
#include <QByteArray>
#include <sqlite3.h>

struct DatabaseHandlerOptions
{
    QString Description;
    QString EncryptionAlgorithm;
    uint16_t EncryptionKeyLength;
    QString KeyDerivationFunction;
};

class DatabaseHandler
{
public:
    DatabaseHandler(const QString& filePath, const QByteArray& password, const DatabaseHandlerOptions* options = nullptr);
    ~DatabaseHandler();

private:
    sqlite3* database = nullptr;
    QByteArray password;

    void setNewDatabaseStructure(const DatabaseHandlerOptions* options = nullptr);
    void createBasicInfoStructure(const DatabaseHandlerOptions* options = nullptr);
};

#endif // DATABASEHANDLER_H
