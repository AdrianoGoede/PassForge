#ifndef CONFIGS_H
#define CONFIGS_H

#define SUPPORTED_HASHING_ALGORITHMS         "SHA-224","SHA-256","SHA-384","SHA-512","MD4","MD5"
#define SUPPORTED_ENCRYPTION_ALGORITHMS      "AES","Serpent","Twofish","Camellia","ChaCha"
#define SUPPORTED_BLOCK_CYPHER_KEY_SETTINGS  "128","192","256"
#define SUPPORTED_STREAM_CYPHER_KEY_SETTINGS ""
#define SUPPORTED_KEY_DERIVATION_FUNCTIONS   "PBKDF2","Scrypt","Argon2id"
#define WORDLISTS_RESOURCES_DIRECTORY        ":/Wordlists"
#define DATABASE_FILE_FILTER                 "PassForge Databases (*.pfdb)"

#define DATABASE_MIN_PASSWORD_LENGTH         8
#define CIPHERTEXT_IV_LENGTH_BYTES           12

#endif // CONFIGS_H
