#include "CredentialEntry.h"
#include "../Configs/Constants.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

CredentialEntry::CredentialEntry() : DatabaseEntry() {}

CredentialEntry::CredentialEntry(const QByteArray& header, const QByteArray& body) : DatabaseEntry(header, DATABASE_ENTRY_TYPE_CREDENTIAL)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->username = obj["username"].toString("");
    this->password = obj["password"].toString("");
    this->notes = obj["notes"].toString("");
    this->url = obj["url"].toString("");
}

CredentialEntry::~CredentialEntry()
{
    Crypto::wipeMemory(this->username.data(), (sizeof(QChar) * this->username.size()));
    Crypto::wipeMemory(this->password.data(), (sizeof(QChar) * this->password.size()));
    Crypto::wipeMemory(this->notes.data(), (sizeof(QChar) * this->notes.size()));
    Crypto::wipeMemory(this->url.data(), (sizeof(QChar) * this->url.size()));
}

QByteArray CredentialEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["sequence"] = this->sequence;
    obj["entryType"] = this->entryType;
    obj["name"] = this->name;
    obj["path"] = this->path;
    obj["username"] = this->username;
    obj["password"] = this->password;
    obj["notes"] = this->notes;
    obj["url"] = this->url;
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

QString CredentialEntry::getUsername() const { return username; }

void CredentialEntry::setUsername(const QString &newUsername) { username = newUsername.trimmed(); }

QString CredentialEntry::getPassword() const { return password; }

void CredentialEntry::setPassword(const QString &newPassword) { password = newPassword; }

QString CredentialEntry::getNotes() const { return notes; }

void CredentialEntry::setNotes(const QString &newNotes) { notes = newNotes.trimmed(); }

QString CredentialEntry::getUrl() const { return url; }

void CredentialEntry::setUrl(const QString &newUrl) { url = newUrl.trimmed(); }
