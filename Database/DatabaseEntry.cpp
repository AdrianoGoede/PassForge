#include "DatabaseEntry.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

DatabaseEntry::DatabaseEntry(int type) : entryId(0), entryType(type) { }

DatabaseEntry::DatabaseEntry(const DatabaseEntry& other)
{
    this->entryId = other.getEntryId();
    this->entryType = other.getEntryType();
    this->name = SecureQByteArray(other.getName().data(), other.getName().size());
    this->path = SecureQByteArray(other.getPath().data(), other.getPath().size());
}

DatabaseEntry::DatabaseEntry(const SecureQByteArray &header, int entryId)
{
    QJsonObject obj = QJsonDocument::fromJson(header).object();
    this->entryId = entryId;
    this->entryType = obj["entryType"].toInt(0);
    this->name = SecureQByteArray(obj["name"].toString("").toUtf8());
    this->path = SecureQByteArray(obj["path"].toString("").toUtf8());
}

DatabaseEntry::~DatabaseEntry()
{
    Crypto::wipeMemory(&this->entryId, sizeof(this->entryId));
    Crypto::wipeMemory(&this->entryType, sizeof(this->entryType));
}

SecureQByteArray DatabaseEntry::getHeaderJson() const
{
    QJsonObject obj;
    obj["entryType"] = this->entryType;
    obj["name"] = this->name.data();
    obj["path"] = this->path.data();
    return SecureQByteArray(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact));
}

SecureQByteArray DatabaseEntry::getBodyJson() const { return SecureQByteArray(); }

int DatabaseEntry::getEntryId() const { return this->entryId; }

void DatabaseEntry::setEntryId(int newEntryId) { this->entryId = newEntryId; }

int DatabaseEntry::getEntryType() const { return this->entryType; }

void DatabaseEntry::setEntryType(int newEntryType) { this->entryType = newEntryType; }

const SecureQByteArray& DatabaseEntry::getPath() const { return this->path; }

void DatabaseEntry::setPath(QByteArray&& newPath) { this->path = SecureQByteArray(std::move(newPath)); }

const SecureQByteArray& DatabaseEntry::getName() const { return this->name; }

void DatabaseEntry::setName(QByteArray&& newName) { this->name = SecureQByteArray(std::move(newName)); }
