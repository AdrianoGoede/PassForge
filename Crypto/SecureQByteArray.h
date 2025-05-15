#ifndef SECUREQBYTEARRAY_H
#define SECUREQBYTEARRAY_H

#include <QByteArray>

class SecureQByteArray : public QByteArray
{
public:
    using QByteArray::QByteArray;
    SecureQByteArray(QByteArray&& source) noexcept;
    ~SecureQByteArray();
};

#endif // SECUREQBYTEARRAY_H
