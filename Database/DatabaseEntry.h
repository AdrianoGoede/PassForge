#ifndef DATABASEENTRY_H
#define DATABASEENTRY_H

#include "../Crypto/SecureQByteArray.h"
#include <QByteArray>

class DatabaseEntry
{
public:
    DatabaseEntry();
    DatabaseEntry(const DatabaseEntry& other);
    DatabaseEntry(const SecureQByteArray& header, int entryId = 0);
    ~DatabaseEntry();

    SecureQByteArray getHeaderJson() const;
    virtual SecureQByteArray getBodyJson() const;

    int getEntryId() const;
    void setEntryId(int newEntryId);

    int getSequence() const;
    void setSequence(int newSequence);

    const SecureQByteArray& getPath() const;
    void setPath(QByteArray&& newPath);

    const SecureQByteArray& getName() const;
    void setName(QByteArray&& newName);

    int getEntryType() const;
    void setEntryType(int newEntryType);

protected:
    int entryId, sequence, entryType;
    SecureQByteArray name, path;
};

#endif // DATABASEENTRY_H
