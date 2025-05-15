#include "SecureQByteArray.h"
#include "../Crypto/Crypto.h"

SecureQByteArray::SecureQByteArray(QByteArray&& other) noexcept : QByteArray(std::move(other)) { Crypto::wipeMemory(other.data(), other.size()); }

SecureQByteArray::~SecureQByteArray() { if (!this->isEmpty()) Crypto::wipeMemory(this->data(), this->size()); }
