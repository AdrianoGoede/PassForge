#ifndef CREDENTIALENTRY_H
#define CREDENTIALENTRY_H

#include "DatabaseEntry.h"
#include "../Crypto/SecureQByteArray.h"
#include <QByteArray>

class CredentialEntry : public DatabaseEntry
{
public:
    CredentialEntry();
    CredentialEntry(const SecureQByteArray& header, const SecureQByteArray& body);
    CredentialEntry(const DatabaseEntry& header, const SecureQByteArray& body);

    SecureQByteArray getBodyJson() const override;

    const SecureQByteArray& getUsername() const;
    void setUsername(QByteArray&& newUsername);

    const SecureQByteArray& getPassword() const;
    void setPassword(QByteArray&& newPassword);

    const SecureQByteArray& getNotes() const;
    void setNotes(QByteArray&& newNotes);

    const SecureQByteArray& getUrl() const;
    void setUrl(QByteArray&& newUrl);

private:
    SecureQByteArray username, password, notes, url;
    void deserializeJson(const QByteArray& body);
};

#endif // CREDENTIALENTRY_H
