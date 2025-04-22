#include "CryptocurrencyEntry.h"
#include "../Configs/Constants.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

CryptocurrencyEntry::CryptocurrencyEntry() : DatabaseEntry() {}

CryptocurrencyEntry::CryptocurrencyEntry(const QByteArray &header, const QByteArray &body) : DatabaseEntry(header, DATABASE_ENTRY_TYPE_CRYPTOCURRENCY)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->cryptocurrencyName = obj["cryptocurrencyName"].toString("");
    this->seed = obj["seed"].toString("");
    this->masterPrivateKey = obj["masterPrivateKey"].toString("");
    this->notes = obj["notes"].toString("");
}

CryptocurrencyEntry::~CryptocurrencyEntry()
{
    Crypto::wipeMemory(this->cryptocurrencyName.data(), (sizeof(QChar) * this->cryptocurrencyName.length()));
    Crypto::wipeMemory(this->seed.data(), (sizeof(QChar) * this->seed.length()));
    Crypto::wipeMemory(this->masterPrivateKey.data(), (sizeof(QChar) * this->masterPrivateKey.length()));
    Crypto::wipeMemory(this->notes.data(), (sizeof(QChar) * this->notes.length()));
}

QByteArray CryptocurrencyEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["sequence"] = this->sequence;
    obj["entryType"] = this->entryType;
    obj["name"] = this->name;
    obj["path"] = this->path;
    obj["cryptocurrencyName"] = this->cryptocurrencyName;
    obj["seed"] = this->seed;
    obj["masterPrivateKey"] = this->masterPrivateKey;
    obj["notes"] = this->notes;
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

QString CryptocurrencyEntry::getCryptocurrencyName() const { return cryptocurrencyName; }

void CryptocurrencyEntry::setCryptocurrencyName(const QString &newCryptocurrencyName) { cryptocurrencyName = newCryptocurrencyName.trimmed(); }

QString CryptocurrencyEntry::getSeed() const { return seed; }

void CryptocurrencyEntry::setSeed(const QString &newSeed) { seed = newSeed.trimmed(); }

QString CryptocurrencyEntry::getMasterPrivateKey() const { return masterPrivateKey; }

void CryptocurrencyEntry::setMasterPrivateKey(const QString &newMasterPrivateKey) { masterPrivateKey = newMasterPrivateKey.trimmed(); }

QString CryptocurrencyEntry::getNotes() const { return notes; }

void CryptocurrencyEntry::setNotes(const QString &newNotes) { notes = newNotes.trimmed(); }
