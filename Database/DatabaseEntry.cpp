#include "DatabaseEntry.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

DatabaseEntry::DatabaseEntry() {}

DatabaseEntry::DatabaseEntry(const QByteArray &header, const int entryType)
{
    QJsonObject obj = QJsonDocument::fromJson(header).object();
    this->sequence = obj["sequence"].toInt(0);
    this->entryType = obj["entryType"].toInt(0);
    this->name = obj["name"].toString("");
    this->path = obj["path"].toString("");
}

DatabaseEntry::~DatabaseEntry()
{
    Crypto::wipeMemory(this->name.data(), (sizeof(QChar) * this->name.length()));
    Crypto::wipeMemory(this->path.data(), (sizeof(QChar) * this->path.length()));
}

QByteArray DatabaseEntry::getHeaderJson() const
{
    QJsonObject obj;
    obj["sequence"] = this->sequence;
    obj["path"] = this->path;
    obj["entryType"] = this->entryType;
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

QByteArray DatabaseEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["sequence"] = this->sequence;
    obj["entryType"] = this->entryType;
    obj["name"] = this->name;
    obj["path"] = this->path;
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

int DatabaseEntry::getSequence() const { return sequence; }

void DatabaseEntry::setSequence(int newSequence){ sequence = newSequence; }

QString DatabaseEntry::getPath() const { return path; }

void DatabaseEntry::setPath(const QString &newPath) { path = newPath.trimmed(); }

QString DatabaseEntry::getName() const { return name; }

void DatabaseEntry::setName(const QString &newName){ name = newName.trimmed(); }
