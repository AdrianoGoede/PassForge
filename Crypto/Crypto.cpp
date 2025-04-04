#include "Crypto.h"
#include <botan-2/botan/botan.h>
#include <botan-2/botan/numthry.h>
#include <botan-2/botan/hex.h>

Crypto::Crypto() {}

void Crypto::wipeMemory(void *address, size_t bytes) { Botan::secure_scrub_memory(address, bytes); }

void Crypto::getRandomUnsignedIntegers(uint32_t *buffer, size_t count, uint32_t min, uint32_t max)
{
    if (!buffer) return;
    Botan::AutoSeeded_RNG rng;
    for (size_t i = 0; i < count; i++) {
        rng.randomize(reinterpret_cast<uint8_t*>(buffer + i), sizeof(*(buffer + i)));
        *(buffer + i) = (min + (*(buffer + i) % (max - min + 1)));
    }
}

std::string Crypto::getHash(const std::string& plainText, const char* algorithm)
{
    Botan::secure_vector<uint8_t> bytes(plainText.cbegin(), plainText.cend());
    std::unique_ptr<Botan::HashFunction> hash = Botan::HashFunction::create(algorithm);
    if (!hash) return std::string(); // TO DO!
    hash->update(bytes);
    bytes = hash->final();
    return Botan::hex_encode(bytes);
}
