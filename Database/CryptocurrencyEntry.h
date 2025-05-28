#ifndef CRYPTOCURRENCYENTRY_H
#define CRYPTOCURRENCYENTRY_H

#include "DatabaseEntry.h"
#include "../Crypto/SecureQByteArray.h"
#include <QByteArray>

class CryptocurrencyEntry : public DatabaseEntry
{
public:
    CryptocurrencyEntry();
    CryptocurrencyEntry(const SecureQByteArray& header, const SecureQByteArray& body);
    CryptocurrencyEntry(const DatabaseEntry& header, const SecureQByteArray& body);

    SecureQByteArray getBodyJson() const override;

    const SecureQByteArray& getCryptocurrencyName() const;
    void setCryptocurrencyName(QByteArray&& newCryptocurrencyName);

    const SecureQByteArray& getSeed() const;
    void setSeed(QByteArray&& newSeed);

    const SecureQByteArray& getMasterPrivateKey() const;
    void setMasterPrivateKey(QByteArray&& newMasterPrivateKey);

    const SecureQByteArray& getNotes() const;
    void setNotes(QByteArray&& newNotes);

private:
    SecureQByteArray cryptocurrencyName, seed, masterPrivateKey, notes;
    void deserializeJson(const QByteArray& body);
};

#endif // CRYPTOCURRENCYENTRY_H
