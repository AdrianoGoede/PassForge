#ifndef DATABASEENTRY_H
#define DATABASEENTRY_H

#include <QString>

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

    QString getPath() const;
    void setPath(const QString &newPath);

    QString getName() const;
    void setName(const QString &newName);

protected:
    int sequence, entryType;
    QString name, path;
};

#endif // DATABASEENTRY_H
