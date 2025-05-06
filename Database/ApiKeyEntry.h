#ifndef APIKEYENTRY_H
#define APIKEYENTRY_H

#include "DatabaseEntry.h"
#include <QByteArray>

class ApiKeyEntry : public DatabaseEntry
{
public:
    ApiKeyEntry();
    ApiKeyEntry(const QByteArray& header, const QByteArray& body);
    ApiKeyEntry(const DatabaseEntry& header, const QByteArray& body);
    ~ApiKeyEntry();

    QByteArray getBodyJson() const override;

    const QByteArray& getUrl() const;
    void setUrl(const QByteArray& newUrl);

    const QByteArray& getKey() const;
    void setKey(const QByteArray& newKey);

    const QByteArray& getNotes() const;
    void setNotes(const QByteArray& newNotes);

private:
    QByteArray url, key, notes;
    void deserializeJson(const QByteArray& body);
};

#endif // APIKEYENTRY_H
