#ifndef CRYPTO_H
#define CRYPTO_H

#include <botan-2/botan/auto_rng.h>
#include <QByteArray>

class Crypto
{
public:
    static void wipeMemory(void* address, size_t bytes);
    static void getRandomUnsignedIntegers(uint32_t* buffer, size_t count, uint32_t min, uint32_t max);
    static QByteArray getHash(const std::string& plainText, const std::string& algorithm);
    static QByteArray generateSalt(size_t length);
    static QByteArray deriveKeyPbkdf2(const char* password, const std::string& salt, size_t keyLength, size_t iterations);
    static QByteArray deriveKeyScrypt(const char* password, size_t passwordLength, const std::string& salt, size_t keyLength, size_t iterations);
    static QByteArray encryptString(const std::string& plaintext, const std::string& key, const std::string& algorithm);
};

#endif // CRYPTO_H
