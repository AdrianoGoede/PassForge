#ifndef CREDENTIALENTRY_H
#define CREDENTIALENTRY_H

#include "DatabaseEntry.h"
#include <QString>

class CredentialEntry : private DatabaseEntry
{
public:
    CredentialEntry();
    CredentialEntry(const QByteArray& header, const QByteArray& body);
    ~CredentialEntry();

    QByteArray getBodyJson() const override;

    QString getUsername() const;
    void setUsername(const QString &newUsername);

    QString getPassword() const;
    void setPassword(const QString &newPassword);

    QString getNotes() const;
    void setNotes(const QString &newNotes);

    QString getUrl() const;
    void setUrl(const QString &newUrl);

private:
    QString username, password, notes, url;
};

#endif // CREDENTIALENTRY_H
