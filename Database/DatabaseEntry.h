#ifndef DATABASEENTRY_H
#define DATABASEENTRY_H

#include <QByteArray>

class DatabaseEntry
{
public:
    DatabaseEntry();
    DatabaseEntry(const QByteArray &header, const int entryType);
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
    int sequence, entryType;
    QByteArray name, path;
};

#endif // DATABASEENTRY_H
