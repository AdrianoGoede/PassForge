#include "Crypto.h"
#include "../Configs/Configs.h"
#include <botan-2/botan/botan.h>
#include <botan-2/botan/numthry.h>
#include <botan-2/botan/pbkdf.h>
#include <botan-2/botan/scrypt.h>
#include <botan-2/botan/argon2.h>
#include <botan-2/botan/hex.h>

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

QByteArray Crypto::getHash(const std::string& plainText, const std::string& algorithm)
{
    std::unique_ptr<Botan::HashFunction> hashFunction = Botan::HashFunction::create(algorithm);
    if (!hashFunction) throw std::runtime_error("");
    Botan::secure_vector<uint8_t> bytes(plainText.cbegin(), plainText.cend());
    hashFunction->update(bytes);
    bytes = hashFunction->final();
    return QByteArray(bytes.data());
}

QByteArray Crypto::generateSalt(size_t length)
{
    Botan::AutoSeeded_RNG rng;
    Botan::secure_vector<uint8_t> salt = rng.random_vec(length / 8);
    return QByteArray(reinterpret_cast<const char*>(salt.data()), static_cast<int>(salt.size()));
}

QByteArray Crypto::deriveKeyPbkdf2(const QByteArray& password, const QByteArray& salt, size_t keyLength, size_t iterations)
{
    std::unique_ptr<Botan::PBKDF> derivationFunction = Botan::PBKDF::create("PBKDF2(SHA-256)");
    if (!derivationFunction) throw std::runtime_error("PBKDF2 not available");
    std::string passwd(password);
    std::vector<uint8_t> saltVec(salt.cbegin(), salt.cend());
    Botan::OctetString key = derivationFunction->derive_key(keyLength, passwd, saltVec.data(), saltVec.size(), iterations);
    wipeMemory(passwd.data(), passwd.length());
    return QByteArray(reinterpret_cast<const char*>(key.begin()), static_cast<int>(key.length()));
}

QByteArray Crypto::deriveKeyScrypt(const QByteArray& password, const QByteArray &salt, size_t keyLength, size_t iterations)
{
    size_t cost_param = (1 << (KEY_DERIVATION_SCRYPT_COST_PARAM + iterations));
    Botan::Scrypt derivationFunction(cost_param, KEY_DERIVATION_SCRYPT_BLOCK_SIZE, KEY_DERIVATION_SCRYPT_PARALLELIZATION);
    Botan::secure_vector<uint8_t> buffer(keyLength);
    derivationFunction.derive_key(buffer.data(), keyLength, password, password.length(), reinterpret_cast<const uint8_t*>(salt.data()), salt.length());
    return QByteArray(reinterpret_cast<const char*>(buffer.data()), static_cast<int>(buffer.size()));
}

QByteArray Crypto::deriveKeyArgon2id(const QByteArray& password, const QByteArray& salt, size_t keyLength, size_t iterations)
{
    Botan::secure_vector<uint8_t> buffer(keyLength);
    Botan::argon2(
        buffer.data(), keyLength,
        password.data(), password.length(),
        reinterpret_cast<const uint8_t*>(salt.data()), salt.length(),
        nullptr, 0, // no secret key
        nullptr, 0, // no additional data
        KEY_DERIVATION_ARGON2_ALGORITHM_TYPE,
        KEY_DERIVATION_ARGON2_PARALLELISM,
        KEY_DERIVATION_ARGON2_MEMORY,
        iterations
    );
    return QByteArray(reinterpret_cast<const char*>(buffer.data()), static_cast<int>(buffer.size()));
}

QByteArray Crypto::encryptString(const QByteArray& plaintext, const QByteArray& key, const QString& algorithm)
{
    std::unique_ptr<Botan::Cipher_Mode> cipher = Botan::Cipher_Mode::create("AES-256/GCM", Botan::ENCRYPTION); // TO DO!
    if (!cipher) throw std::runtime_error("Encryption algorithm not supported");

    Botan::AutoSeeded_RNG rng;
    Botan::secure_vector<uint8_t> buffer(plaintext.cbegin(), plaintext.cend());
    Botan::secure_vector<uint8_t> initializationVector = rng.random_vec(CIPHERTEXT_IV_LENGTH_BYTES);

    cipher->set_key(Botan::secure_vector<uint8_t>(key.cbegin(), key.cend()));
    cipher->start(initializationVector);
    cipher->finish(buffer);

    return (Botan::hex_encode(initializationVector) + Botan::hex_encode(buffer)).data();
}
