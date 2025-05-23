#ifndef SECUREQBYTEARRAY_H
#define SECUREQBYTEARRAY_H

#include <QByteArray>
#include <QList>

class SecureQByteArray : public QByteArray
{
public:
    using QByteArray::QByteArray;
    SecureQByteArray(QByteArray&& source) noexcept;
    ~SecureQByteArray();
    QList<SecureQByteArray> secureSplit(char sep) const;
};

#endif // SECUREQBYTEARRAY_H
