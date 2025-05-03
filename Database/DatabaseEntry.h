#ifndef DATABASEENTRY_H
#define DATABASEENTRY_H

#include <QByteArray>

class DatabaseEntry
{
public:
    DatabaseEntry();
    DatabaseEntry(const QByteArray &header, int entryId = 0);
    ~DatabaseEntry();

    QByteArray getHeaderJson() const;
    virtual QByteArray getBodyJson() const;

    int getSequence() const;
    void setSequence(int newSequence);

    const QByteArray& getPath() const;
    void setPath(const QByteArray& newPath);

    const QByteArray& getName() const;
    void setName(const QByteArray& newName);

protected:
    int entryId, sequence, entryType;
    QByteArray name, path;
};

#endif // DATABASEENTRY_H
