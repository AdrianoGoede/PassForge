#ifndef CREDENTIALENTRY_H
#define CREDENTIALENTRY_H

#include "DatabaseEntry.h"
#include <QByteArray>

class CredentialEntry : private DatabaseEntry
{
public:
    CredentialEntry();
    CredentialEntry(const QByteArray& header, const QByteArray& body);
    ~CredentialEntry();

    QByteArray getBodyJson() const override;

    const QByteArray& getUsername() const;
    void setUsername(const QByteArray& newUsername);

    const QByteArray& getPassword() const;
    void setPassword(const QByteArray& newPassword);

    const QByteArray& getNotes() const;
    void setNotes(const QByteArray& newNotes);

    const QByteArray& getUrl() const;
    void setUrl(const QByteArray& newUrl);

private:
    QByteArray username, password, notes, url;
};

#endif // CREDENTIALENTRY_H
