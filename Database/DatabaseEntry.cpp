#include "DatabaseEntry.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

DatabaseEntry::DatabaseEntry() {}

DatabaseEntry::DatabaseEntry(const QByteArray &header, int entryId)
{
    QJsonObject obj = QJsonDocument::fromJson(header).object();
    this->entryId = entryId;
    this->sequence = obj["sequence"].toInt(0);
    this->entryType = obj["entryType"].toInt(0);
    this->name = obj["name"].toString("").toUtf8();
    this->path = obj["path"].toString("").toUtf8();
}

DatabaseEntry::~DatabaseEntry()
{
    Crypto::wipeMemory(this->name.data(), this->name.length());
    Crypto::wipeMemory(this->path.data(), this->path.length());
}

QByteArray DatabaseEntry::getHeaderJson() const
{
    QJsonObject obj;
    obj["sequence"] = this->sequence;
    obj["entryType"] = this->entryType;
    obj["name"] = this->name.data();
    obj["path"] = this->path.data();
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

QByteArray DatabaseEntry::getBodyJson() const { return QByteArray(); }

int DatabaseEntry::getSequence() const { return this->sequence; }

void DatabaseEntry::setSequence(int newSequence) { this->sequence = newSequence; }

const QByteArray& DatabaseEntry::getPath() const { return this->path; }

void DatabaseEntry::setPath(const QByteArray& newPath)
{
    Crypto::wipeMemory(this->path.data(), this->path.length());
    this->path = newPath.trimmed();
}

const QByteArray& DatabaseEntry::getName() const { return this->name; }

void DatabaseEntry::setName(const QByteArray& newName)
{
    Crypto::wipeMemory(this->name.data(), this->name.length());
    this->name = newName.trimmed();
}
