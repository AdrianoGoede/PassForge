#include "CredentialEntry.h"
#include <QJsonDocument>
#include <QJsonObject>

CredentialEntry::CredentialEntry() : DatabaseEntry() { }

CredentialEntry::CredentialEntry(const SecureQByteArray& header, const SecureQByteArray& body) : DatabaseEntry(header) { this->deserializeJson(body); }

CredentialEntry::CredentialEntry(const DatabaseEntry& header, const SecureQByteArray& body) : DatabaseEntry(header) { this->deserializeJson(body); }

SecureQByteArray CredentialEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["username"] = this->username.data();
    obj["password"] = this->password.data();
    obj["notes"] = this->notes.data();
    obj["url"] = this->url.data();
    return SecureQByteArray(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact));
}

const SecureQByteArray& CredentialEntry::getUsername() const { return this->username; }

void CredentialEntry::setUsername(QByteArray& newUsername) { this->username = SecureQByteArray(std::move(newUsername)); }

const SecureQByteArray& CredentialEntry::getPassword() const { return this->password; }

void CredentialEntry::setPassword(QByteArray& newPassword) { this->password = SecureQByteArray(std::move(newPassword)); }

const SecureQByteArray& CredentialEntry::getNotes() const { return this->notes; }

void CredentialEntry::setNotes(QByteArray& newNotes) { this->notes = SecureQByteArray(std::move(newNotes)); }

const SecureQByteArray& CredentialEntry::getUrl() const { return this->url; }

void CredentialEntry::setUrl(QByteArray& newUrl) { this->url = SecureQByteArray(std::move(newUrl)); }

void CredentialEntry::deserializeJson(const QByteArray& body)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->username = SecureQByteArray(obj["username"].toString("").toUtf8());
    this->password = SecureQByteArray(obj["password"].toString("").toUtf8());
    this->notes = SecureQByteArray(obj["notes"].toString("").toUtf8());
    this->url = SecureQByteArray(obj["url"].toString("").toUtf8());
}
