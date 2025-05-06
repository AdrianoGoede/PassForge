#include "CredentialEntry.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

CredentialEntry::CredentialEntry() : DatabaseEntry() {}

CredentialEntry::CredentialEntry(const QByteArray& header, const QByteArray& body) : DatabaseEntry(header) { this->deserializeJson(body); }

CredentialEntry::CredentialEntry(const DatabaseEntry &header, const QByteArray &body) : DatabaseEntry(header) { this->deserializeJson(body); }

CredentialEntry::~CredentialEntry()
{
    Crypto::wipeMemory(this->username.data(), this->username.size());
    Crypto::wipeMemory(this->password.data(), this->password.size());
    Crypto::wipeMemory(this->notes.data(), this->notes.size());
    Crypto::wipeMemory(this->url.data(), this->url.size());
}

QByteArray CredentialEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["username"] = this->username.data();
    obj["password"] = this->password.data();
    obj["notes"] = this->notes.data();
    obj["url"] = this->url.data();
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

const QByteArray& CredentialEntry::getUsername() const { return this->username; }

void CredentialEntry::setUsername(const QByteArray& newUsername)
{
    Crypto::wipeMemory(this->username.data(), this->username.length());
    this->username = newUsername.trimmed();
}

const QByteArray& CredentialEntry::getPassword() const { return this->password; }

void CredentialEntry::setPassword(const QByteArray& newPassword)
{
    Crypto::wipeMemory(this->password.data(), this->password.length());
    this->password = newPassword;
}

const QByteArray& CredentialEntry::getNotes() const { return this->notes; }

void CredentialEntry::setNotes(const QByteArray& newNotes)
{
    Crypto::wipeMemory(this->notes.data(), this->notes.length());
    this->notes = newNotes;
}

const QByteArray& CredentialEntry::getUrl() const { return this->url; }

void CredentialEntry::setUrl(const QByteArray& newUrl)
{
    Crypto::wipeMemory(this->url.data(), this->url.length());
    this->url = newUrl.trimmed();
}

void CredentialEntry::deserializeJson(const QByteArray &body)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->username = obj["username"].toString("").toUtf8();
    this->password = obj["password"].toString("").toUtf8();
    this->notes = obj["notes"].toString("").toUtf8();
    this->url = obj["url"].toString("").toUtf8();
}
