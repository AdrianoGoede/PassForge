#include "Crypto.h"
#include "../Configs/Configs.h"
#include <botan-2/botan/botan.h>
#include <botan-2/botan/numthry.h>
#include <botan-2/botan/pbkdf.h>
#include <botan-2/botan/scrypt.h>
#include <botan-2/botan/argon2.h>
#include <botan-2/botan/hex.h>
#include <botan/aead.h>
#include <QString>
#include <QStringList>

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

QByteArray Crypto::generateRandomKey(size_t keyLength)
{
    Botan::AutoSeeded_RNG rng;
    Botan::secure_vector<uint8_t> key = rng.random_vec(keyLength);
    return QByteArray(reinterpret_cast<const char*>(key.data()), static_cast<int>(key.size()));
}

QByteArray Crypto::getHash(const QByteArray& plainText, const QString& algorithm)
{
    std::unique_ptr<Botan::HashFunction> hashFunction = Botan::HashFunction::create(algorithm.toStdString());
    if (!hashFunction) throw std::runtime_error("Algorithm not supported");
    Botan::secure_vector<uint8_t> bytes(plainText.cbegin(), plainText.cend());
    hashFunction->update(bytes);
    bytes = hashFunction->final();
    return QByteArray(reinterpret_cast<const char*>(bytes.data()), static_cast<int>(bytes.size()));
}

QByteArray Crypto::generateSalt(size_t length)
{
    Botan::AutoSeeded_RNG rng;
    Botan::secure_vector<uint8_t> salt = rng.random_vec(length / 8);
    return QByteArray(reinterpret_cast<const char*>(salt.data()), static_cast<int>(salt.size()));
}

QByteArray Crypto::deriveKey(const QByteArray &password, const QByteArray &salt, size_t keyLength, size_t iterations, const QString &algorithm)
{
    if (algorithm == "PBKDF2")
        return deriveKeyPbkdf2(password, salt, keyLength, iterations);
    if (algorithm == "Scrypt")
        return deriveKeyScrypt(password, salt, keyLength, iterations);
    if (algorithm == "Argon2id")
        return deriveKeyArgon2id(password, salt, keyLength, iterations);
    throw std::runtime_error("Key derivation function not supported");
}

QByteArray Crypto::encrypt(const QByteArray& plaintext, const QByteArray& key, const QString& cipherSetting)
{
    if (QStringList({ CIPHER_SETTINGS_CHACHA20 }).contains(cipherSetting))
        return encryptWithStreamCipher(plaintext, key, cipherSetting);
    return encryptWithBlockCipher(plaintext, key, cipherSetting);
}

QByteArray Crypto::decrypt(const QByteArray &ciphertext, const QByteArray &key, const QString &cipherSetting)
{
    if (QStringList({ CIPHER_SETTINGS_CHACHA20 }).contains(cipherSetting))
        return decryptWithStreamCipher(ciphertext, key, cipherSetting);
    return decryptWithBlockCipher(ciphertext, key, cipherSetting);
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

QByteArray Crypto::encryptWithBlockCipher(const QByteArray& plaintext, const QByteArray& key, const QString& cipherSetting)
{
    size_t keyLength = (key.length() * 8);
    if (!QStringList({ SUPPORTED_BLOCK_CYPHER_KEY_SETTINGS }).contains(std::to_string(keyLength)))
        throw std::runtime_error(std::to_string(keyLength) + " bits key setting not supported");
    std::unique_ptr<Botan::AEAD_Mode> cipher = Botan::AEAD_Mode::create(cipherSetting.toStdString(), Botan::ENCRYPTION);
    if (!cipher) throw std::runtime_error("Algorithm not supported");
    cipher->set_key(Botan::secure_vector<uint8_t>(key.cbegin(), key.cend()));

    Botan::AutoSeeded_RNG rng;
    Botan::secure_vector<uint8_t> buffer(plaintext.cbegin(), plaintext.cend());
    Botan::secure_vector<uint8_t> initializationVector = rng.random_vec(cipher->default_nonce_length());

    try {
        cipher->start(initializationVector);
        cipher->finish(buffer);
        return QByteArray(reinterpret_cast<char*>(initializationVector.data()), static_cast<int>(initializationVector.size()))
            .append(QByteArray(reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size())));
    }
    catch (const std::exception& ex) { throw std::runtime_error("Encryption failed - " + std::string(ex.what())); }
}

QByteArray Crypto::decryptWithBlockCipher(const QByteArray& ciphertext, const QByteArray& key, const QString& cipherSetting)
{
    size_t keyLength = (key.length() * 8);
    if (!QStringList({SUPPORTED_BLOCK_CYPHER_KEY_SETTINGS}).contains(QString::number(keyLength)))
        throw std::runtime_error(std::to_string(keyLength) + " bits key setting not supported");
    std::unique_ptr<Botan::AEAD_Mode> cipher = Botan::AEAD_Mode::create(cipherSetting.toStdString(), Botan::DECRYPTION);
    if (!cipher) throw std::runtime_error("Algorithm not supported");
    cipher->set_key(Botan::secure_vector<uint8_t>(key.cbegin(), key.cend()));

    Botan::secure_vector<uint8_t> buffer((ciphertext.cbegin() + cipher->default_nonce_length()), ciphertext.cend());
    Botan::secure_vector<uint8_t> initializationVector(ciphertext.cbegin(), (ciphertext.cbegin() + cipher->default_nonce_length()));

    try {
        cipher->start(initializationVector);
        cipher->finish(buffer);
        return QByteArray(reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()));
    }
    catch (const std::exception& ex) { throw std::runtime_error("Decryption failed - " + std::string(ex.what())); }
}

QByteArray Crypto::encryptWithStreamCipher(const QByteArray& plaintext, const QByteArray& key, const QString& cipherSetting)
{
    if (key.length() != 32) throw std::runtime_error("Key must be 256 bits long");
    std::unique_ptr<Botan::StreamCipher> cipher = Botan::StreamCipher::create(cipherSetting.toStdString());
    if (!cipher) throw std::runtime_error("Algorithm not supported");
    cipher->set_key(Botan::secure_vector<uint8_t>(key.cbegin(), key.cend()));

    Botan::AutoSeeded_RNG rng;
    Botan::secure_vector<uint8_t> buffer(plaintext.cbegin(), plaintext.cend());
    Botan::secure_vector<uint8_t> initializationVector = rng.random_vec(cipher->default_iv_length());

    try {
        cipher->set_iv(initializationVector.data(), initializationVector.size());
        cipher->encrypt(buffer);
        return QByteArray(reinterpret_cast<char*>(initializationVector.data()), static_cast<int>(initializationVector.size()))
            .append(QByteArray(reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size())));
    }
    catch (const std::exception& ex) { throw std::runtime_error("Encryption failed - " + std::string(ex.what())); }
}

QByteArray Crypto::decryptWithStreamCipher(const QByteArray& ciphertext, const QByteArray& key, const QString& cipherSetting)
{
    if (key.length() != 32) throw std::runtime_error("Key must be 256 bits long");
    std::unique_ptr<Botan::StreamCipher> cipher = Botan::StreamCipher::create(cipherSetting.toStdString());
    if (!cipher) throw std::runtime_error("Algorithm not supported");
    cipher->set_key(Botan::secure_vector<uint8_t>(key.cbegin(), key.cend()));

    Botan::secure_vector<uint8_t> buffer((ciphertext.cbegin() + cipher->default_iv_length()), ciphertext.cend());
    Botan::secure_vector<uint8_t> initializationVector(ciphertext.cbegin(), (ciphertext.cbegin() + cipher->default_iv_length()));

    try {
        cipher->set_iv(initializationVector.data(), initializationVector.size());
        cipher->encrypt(buffer);
        return QByteArray(reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()));
    }
    catch (const std::exception& ex) { throw std::runtime_error("Decryption failed - " + std::string(ex.what())); }
}
