#ifndef CRYPTOCURRENCYENTRY_H
#define CRYPTOCURRENCYENTRY_H

#include "DatabaseEntry.h"
#include <QByteArray>

class CryptocurrencyEntry : private DatabaseEntry
{
public:
    CryptocurrencyEntry();
    CryptocurrencyEntry(const QByteArray& header, const QByteArray& body);
    ~CryptocurrencyEntry();

    QByteArray getBodyJson() const override;

    const QByteArray& getCryptocurrencyName() const;
    void setCryptocurrencyName(const QByteArray& newCryptocurrencyName);

    const QByteArray& getSeed() const;
    void setSeed(const QByteArray& newSeed);

    const QByteArray& getMasterPrivateKey() const;
    void setMasterPrivateKey(const QByteArray& newMasterPrivateKey);

    const QByteArray& getNotes() const;
    void setNotes(const QByteArray& newNotes);

private:
    QByteArray cryptocurrencyName, seed, masterPrivateKey, notes;
};

#endif // CRYPTOCURRENCYENTRY_H
