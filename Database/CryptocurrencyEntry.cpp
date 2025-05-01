#include "CryptocurrencyEntry.h"
#include "../Configs/Constants.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

CryptocurrencyEntry::CryptocurrencyEntry() : DatabaseEntry() {}

CryptocurrencyEntry::CryptocurrencyEntry(const QByteArray& header, const QByteArray& body) : DatabaseEntry(header, DATABASE_ENTRY_TYPE_CRYPTOCURRENCY)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->cryptocurrencyName = obj["cryptocurrencyName"].toString("").toUtf8();
    this->seed = obj["seed"].toString("").toUtf8();
    this->masterPrivateKey = obj["masterPrivateKey"].toString("").toUtf8();
    this->notes = obj["notes"].toString("").toUtf8();
}

CryptocurrencyEntry::~CryptocurrencyEntry()
{
    Crypto::wipeMemory(this->cryptocurrencyName.data(), this->cryptocurrencyName.length());
    Crypto::wipeMemory(this->seed.data(), this->seed.length());
    Crypto::wipeMemory(this->masterPrivateKey.data(), this->masterPrivateKey.length());
    Crypto::wipeMemory(this->notes.data(), this->notes.length());
}

QByteArray CryptocurrencyEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["cryptocurrencyName"] = this->cryptocurrencyName.data();
    obj["seed"] = this->seed.data();
    obj["masterPrivateKey"] = this->masterPrivateKey.data();
    obj["notes"] = this->notes.data();
    return QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);
}

const QByteArray& CryptocurrencyEntry::getCryptocurrencyName() const { return this->cryptocurrencyName; }

void CryptocurrencyEntry::setCryptocurrencyName(const QByteArray& newCryptocurrencyName)
{
    Crypto::wipeMemory(this->cryptocurrencyName.data(), this->cryptocurrencyName.length());
    this->cryptocurrencyName = newCryptocurrencyName.trimmed();
}

const QByteArray& CryptocurrencyEntry::getSeed() const { return this->seed; }

void CryptocurrencyEntry::setSeed(const QByteArray& newSeed)
{
    Crypto::wipeMemory(this->seed.data(), this->seed.length());
    this->seed = newSeed;
}

const QByteArray& CryptocurrencyEntry::getMasterPrivateKey() const { return this->masterPrivateKey; }

void CryptocurrencyEntry::setMasterPrivateKey(const QByteArray& newMasterPrivateKey)
{
    Crypto::wipeMemory(this->masterPrivateKey.data(), this->masterPrivateKey.length());
    this->masterPrivateKey = newMasterPrivateKey;
}

const QByteArray& CryptocurrencyEntry::getNotes() const { return this->notes; }

void CryptocurrencyEntry::setNotes(const QByteArray& newNotes)
{
    Crypto::wipeMemory(this->notes.data(), this->notes.length());
    this->notes = newNotes;
}
