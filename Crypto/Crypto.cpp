#include "Crypto.h"
#include <botan-2/botan/botan.h>
#include <botan-2/botan/numthry.h>

Crypto::Crypto() {}

void Crypto::getRandomUnsignedIntegers(uint32_t *buffer, size_t count, uint32_t min, uint32_t max)
{
    if (!buffer) return;
    Botan::AutoSeeded_RNG rng;
    for (size_t i = 0; i < count; i++) {
        rng.randomize(reinterpret_cast<uint8_t*>(buffer + i), sizeof(*(buffer + i)));
        *(buffer + i) = (min + (*(buffer + i) % (max - min + 1)));
    }
}
