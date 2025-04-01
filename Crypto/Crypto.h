#ifndef CRYPTO_H
#define CRYPTO_H

#include <botan-2/botan/auto_rng.h>

class Crypto
{
public:
    Crypto();
    static void getRandomUnsignedIntegers(uint32_t* buffer, size_t count, uint32_t min, uint32_t max);
};

#endif // CRYPTO_H
