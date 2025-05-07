#ifndef CRYPTO_H
#define CRYPTO_H

#include <botan-2/botan/auto_rng.h>
#include <QByteArray>

class Crypto
{
public:
    static void wipeMemory(void* address, size_t bytes);
    static void getRandomUnsignedIntegers(uint32_t* buffer, size_t count, uint32_t min, uint32_t max);
    static QByteArray generateRandomKey(size_t keyLength);
    static QByteArray getHash(const QByteArray& plainText, const QString& algorithm);
    static QByteArray generateSalt(size_t length);
    static QByteArray deriveKey(const QByteArray& password, const QByteArray& salt, size_t keyLength, size_t iterations, const QString& algorithm);
    static QByteArray encrypt(const QByteArray& plaintext, const QByteArray& key, const QString& cipherSetting);
    static QByteArray decrypt(const QByteArray& ciphertext, const QByteArray& key, const QString& cipherSetting);

private:
    static QByteArray deriveKeyPbkdf2(const QByteArray& password, const QByteArray& salt, size_t keyLength, size_t iterations);
    static QByteArray deriveKeyScrypt(const QByteArray& password, const QByteArray& salt, size_t keyLength, size_t iterations);
    static QByteArray deriveKeyArgon2id(const QByteArray& password, const QByteArray& salt, size_t keyLength, size_t iterations);
    static QByteArray encryptWithBlockCipher(const QByteArray& plaintext, const QByteArray& key, const QString& cipherSetting);
    static QByteArray decryptWithBlockCipher(const QByteArray& ciphertext, const QByteArray& key, const QString& cipherSetting);
    static QByteArray encryptWithStreamCipher(const QByteArray& plaintext, const QByteArray& key, const QString& cipherSetting);
    static QByteArray decryptWithStreamCipher(const QByteArray& ciphertext, const QByteArray& key, const QString& cipherSetting);
};

#endif // CRYPTO_H
