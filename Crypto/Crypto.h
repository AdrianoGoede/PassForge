#ifndef CRYPTO_H
#define CRYPTO_H

#include "../Crypto/SecureQByteArray.h"
#include <botan-2/botan/auto_rng.h>
#include <QByteArray>

class Crypto
{
public:
    static void wipeMemory(void* address, size_t bytes);
    static void getRandomUnsignedIntegers(uint32_t* buffer, size_t count, uint32_t min, uint32_t max);
    static SecureQByteArray generateRandomKey(size_t keyLength);
    static SecureQByteArray getHash(const SecureQByteArray& plainText, const QString& algorithm);
    static SecureQByteArray generateSalt(size_t length);
    static SecureQByteArray deriveKey(const SecureQByteArray& password, const SecureQByteArray& salt, size_t keyLength, size_t iterations, const QString& algorithm);
    static SecureQByteArray encrypt(const SecureQByteArray& plaintext, const SecureQByteArray& key, const QString& cipherSetting);
    static SecureQByteArray decrypt(const SecureQByteArray& ciphertext, const SecureQByteArray& key, const QString& cipherSetting);

private:
    static SecureQByteArray deriveKeyPbkdf2(const SecureQByteArray& password, const SecureQByteArray& salt, size_t keyLength, size_t iterations);
    static SecureQByteArray deriveKeyScrypt(const SecureQByteArray& password, const SecureQByteArray& salt, size_t keyLength, size_t iterations);
    static SecureQByteArray deriveKeyArgon2id(const SecureQByteArray& password, const SecureQByteArray& salt, size_t keyLength, size_t iterations);
    static SecureQByteArray encryptWithBlockCipher(const SecureQByteArray& plaintext, const SecureQByteArray& key, const QString& cipherSetting);
    static SecureQByteArray decryptWithBlockCipher(const SecureQByteArray& ciphertext, const SecureQByteArray& key, const QString& cipherSetting);
    static SecureQByteArray encryptWithStreamCipher(const SecureQByteArray& plaintext, const SecureQByteArray& key, const QString& cipherSetting);
    static SecureQByteArray decryptWithStreamCipher(const SecureQByteArray& ciphertext, const SecureQByteArray& key, const QString& cipherSetting);
};

#endif // CRYPTO_H
