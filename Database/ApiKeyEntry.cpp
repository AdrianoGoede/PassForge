#include "ApiKeyEntry.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

ApiKeyEntry::ApiKeyEntry() {}

ApiKeyEntry::ApiKeyEntry(const QByteArray &header, const QByteArray &body) : DatabaseEntry(header)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->url = obj["url"].toString("").toUtf8();
    this->key = obj["key"].toString("").toUtf8();
    this->notes = obj["notes"].toString("").toUtf8();
}

ApiKeyEntry::~ApiKeyEntry()
{
    Crypto::wipeMemory(this->url.data(), this->url.length());
    Crypto::wipeMemory(this->key.data(), this->key.length());
    Crypto::wipeMemory(this->notes.data(), this->notes.length());
}

QByteArray ApiKeyEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["url"] = this->url.data();
    obj["key"] = this->key.data();
    obj["notes"] = this->notes.data();
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

const QByteArray& ApiKeyEntry::getUrl() const { return this->url; }

void ApiKeyEntry::setUrl(const QByteArray &newUrl)
{
    Crypto::wipeMemory(this->url.data(), this->url.length());
    this->url = newUrl;
}

const QByteArray& ApiKeyEntry::getKey() const { return this->key; }

void ApiKeyEntry::setKey(const QByteArray &newKey)
{
    Crypto::wipeMemory(this->key.data(), this->key.length());
    this->key = newKey;
}

const QByteArray& ApiKeyEntry::getNotes() const { return this->notes; }

void ApiKeyEntry::setNotes(const QByteArray &newNotes)
{
    Crypto::wipeMemory(this->notes.data(), this->notes.length());
    this->notes = newNotes;
}
