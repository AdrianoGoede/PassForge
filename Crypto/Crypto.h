#ifndef CRYPTO_H
#define CRYPTO_H

#include <botan-2/botan/auto_rng.h>

class Crypto
{
public:
    static void wipeMemory(void* address, size_t bytes);
    static void getRandomUnsignedIntegers(uint32_t* buffer, size_t count, uint32_t min, uint32_t max);
    static std::string getHash(const std::string& plainText, const char* algorithm);
    static std::string generateSalt(size_t length);
    static std::string deriveKeyPbkdf2(const char *password, size_t passwordLength, const std::string& salt, size_t keyLength, size_t iterations);
    static std::string encryptString(const std::string& plaintext, const std::string& key, const std::string& algorithm);
};

#endif // CRYPTO_H
