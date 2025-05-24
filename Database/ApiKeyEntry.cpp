#include "ApiKeyEntry.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "../Configs/Constants.h"

ApiKeyEntry::ApiKeyEntry() : DatabaseEntry(DATABASE_ENTRY_TYPE_API_KEY) { }

ApiKeyEntry::ApiKeyEntry(const SecureQByteArray& header, const SecureQByteArray& body) : DatabaseEntry(header) { this->deserializeJson(body); }

ApiKeyEntry::ApiKeyEntry(const DatabaseEntry& header, const SecureQByteArray& body) : DatabaseEntry(header) { this->deserializeJson(body); }

SecureQByteArray ApiKeyEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["url"] = this->url.data();
    obj["key"] = this->key.data();
    obj["notes"] = this->notes.data();
    return SecureQByteArray(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact));
}

const SecureQByteArray& ApiKeyEntry::getUrl() const { return this->url; }

void ApiKeyEntry::setUrl(QByteArray&& newUrl) { this->url = SecureQByteArray(std::move(newUrl)); }

const SecureQByteArray& ApiKeyEntry::getKey() const { return this->key; }

void ApiKeyEntry::setKey(QByteArray&& newKey) { this->key = SecureQByteArray(std::move(newKey)); }

const SecureQByteArray& ApiKeyEntry::getNotes() const { return this->notes; }

void ApiKeyEntry::setNotes(QByteArray&& newNotes) { this->notes = SecureQByteArray(std::move(newNotes)); }

void ApiKeyEntry::deserializeJson(const QByteArray& body)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->url = SecureQByteArray(obj["url"].toString("").toUtf8());
    this->key = SecureQByteArray(obj["key"].toString("").toUtf8());
    this->notes = SecureQByteArray(obj["notes"].toString("").toUtf8());
}
