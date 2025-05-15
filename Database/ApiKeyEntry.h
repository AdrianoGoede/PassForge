#ifndef APIKEYENTRY_H
#define APIKEYENTRY_H

#include "DatabaseEntry.h"
#include "../Crypto/SecureQByteArray.h"
#include <QByteArray>

class ApiKeyEntry : public DatabaseEntry
{
public:
    ApiKeyEntry();
    ApiKeyEntry(const SecureQByteArray& header, const SecureQByteArray& body);
    ApiKeyEntry(const DatabaseEntry& header, const SecureQByteArray& body);

    SecureQByteArray getBodyJson() const override;

    const SecureQByteArray& getUrl() const;
    void setUrl(QByteArray&& newUrl);

    const SecureQByteArray& getKey() const;
    void setKey(QByteArray&& newKey);

    const SecureQByteArray& getNotes() const;
    void setNotes(QByteArray&& newNotes);

private:
    SecureQByteArray url, key, notes;
    void deserializeJson(const QByteArray& body);
};

#endif // APIKEYENTRY_H
