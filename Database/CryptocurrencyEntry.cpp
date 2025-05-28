#include "CryptocurrencyEntry.h"
#include "../Configs/Constants.h"
#include <QJsonDocument>
#include <QJsonObject>

CryptocurrencyEntry::CryptocurrencyEntry() : DatabaseEntry(DATABASE_ENTRY_TYPE_CRYPTOCURRENCY) {}

CryptocurrencyEntry::CryptocurrencyEntry(const SecureQByteArray& header, const SecureQByteArray& body) : DatabaseEntry(header) { this->deserializeJson(body); }

CryptocurrencyEntry::CryptocurrencyEntry(const DatabaseEntry &header, const SecureQByteArray &body) : DatabaseEntry(header) { this->deserializeJson(body); }

SecureQByteArray CryptocurrencyEntry::getBodyJson() const
{
    QJsonObject obj;
    obj["cryptocurrencyName"] = this->cryptocurrencyName.data();
    obj["seed"] = this->seed.data();
    obj["masterPrivateKey"] = this->masterPrivateKey.data();
    obj["notes"] = this->notes.data();
    return SecureQByteArray(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact));
}

const SecureQByteArray& CryptocurrencyEntry::getCryptocurrencyName() const { return this->cryptocurrencyName; }

void CryptocurrencyEntry::setCryptocurrencyName(QByteArray&& newCryptocurrencyName) { this->cryptocurrencyName = SecureQByteArray(std::move(newCryptocurrencyName)); }

const SecureQByteArray& CryptocurrencyEntry::getSeed() const { return this->seed; }

void CryptocurrencyEntry::setSeed(QByteArray&& newSeed) { this->seed = SecureQByteArray(std::move(newSeed)); }

const SecureQByteArray& CryptocurrencyEntry::getMasterPrivateKey() const { return this->masterPrivateKey; }

void CryptocurrencyEntry::setMasterPrivateKey(QByteArray&& newMasterPrivateKey) { this->masterPrivateKey = SecureQByteArray(std::move(newMasterPrivateKey)); }

const SecureQByteArray& CryptocurrencyEntry::getNotes() const { return this->notes; }

void CryptocurrencyEntry::setNotes(QByteArray&& newNotes) { this->notes = SecureQByteArray(std::move(newNotes)); }

void CryptocurrencyEntry::deserializeJson(const QByteArray &body)
{
    QJsonObject obj = QJsonDocument::fromJson(body).object();
    this->cryptocurrencyName = SecureQByteArray(obj["cryptocurrencyName"].toString("").toUtf8());
    this->seed = SecureQByteArray(obj["seed"].toString("").toUtf8());
    this->masterPrivateKey = SecureQByteArray(obj["masterPrivateKey"].toString("").toUtf8());
    this->notes = SecureQByteArray(obj["notes"].toString("").toUtf8());
}
