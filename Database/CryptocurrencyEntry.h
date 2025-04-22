#ifndef CRYPTOCURRENCYENTRY_H
#define CRYPTOCURRENCYENTRY_H

#include "DatabaseEntry.h"
#include <QByteArray>
#include <QString>

class CryptocurrencyEntry : private DatabaseEntry
{
public:
    CryptocurrencyEntry();
    CryptocurrencyEntry(const QByteArray& header, const QByteArray& body);
    ~CryptocurrencyEntry();

    QByteArray getBodyJson() const override;

    QString getCryptocurrencyName() const;
    void setCryptocurrencyName(const QString &newCryptocurrencyName);

    QString getSeed() const;
    void setSeed(const QString &newSeed);

    QString getMasterPrivateKey() const;
    void setMasterPrivateKey(const QString &newMasterPrivateKey);

    QString getNotes() const;
    void setNotes(const QString &newNotes);

private:
    QString cryptocurrencyName, seed, masterPrivateKey, notes;
};

#endif // CRYPTOCURRENCYENTRY_H
