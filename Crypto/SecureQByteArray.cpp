#include "SecureQByteArray.h"
#include "../Crypto/Crypto.h"

SecureQByteArray::SecureQByteArray(QByteArray&& other) noexcept : QByteArray(std::move(other)) { Crypto::wipeMemory(other.data(), other.size()); }

SecureQByteArray::~SecureQByteArray() { if (!this->isEmpty()) Crypto::wipeMemory(this->data(), this->size()); }

QList<SecureQByteArray> SecureQByteArray::secureSplit(char sep) const
{
    QList<SecureQByteArray> results;
    int start = 0, end = 0;

    while ((end = this->indexOf(sep, start)) != -1) {
        results.push_back(SecureQByteArray(this->mid(start, (end - start))));
        start = (end + 1);
    }

    results.push_back(SecureQByteArray(this->mid(start)));
    return results;
}
