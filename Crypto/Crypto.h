#ifndef CRYPTO_H
#define CRYPTO_H

#include <botan-2/botan/auto_rng.h>

class Crypto
{
public:
    Crypto();
    static void wipeMemory(void* address, size_t bytes);
    static void getRandomUnsignedIntegers(uint32_t* buffer, size_t count, uint32_t min, uint32_t max);
    static std::string getHash(const std::string& plainText, const char* algorithm);
};

#endif // CRYPTO_H
