#ifndef CONSTANTS_H
#define CONSTANTS_H

#define UPPER_CASE_CHARACTERS                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER_CASE_CHARACTERS                     "abcdefghijklmnopqrstuvwxyz"
#define NUMBER_CHARACTERS                         "0123456789"
#define LOGOGRAM_CHARACTERS                       "@#$%&`^~"
#define PUNCTUATION_CHARACTERS                    ".,:;!?"
#define DASH_SLASH_CHARACTERS                     "\\/|_-"
#define MATH_SYMBOL_CHARACTERS                    "<>*+-"
#define BRACE_CHARACTERS                          "{[()]}"
#define QUOTE_CHARACTERS                          "'\""

#define DATABASE_BASIC_DATA_DESCRIPTION           "Description"
#define DATABASE_BASIC_DATA_ENC_ALGORITHM         "EncryptionAlgorithm"
#define DATABASE_BASIC_DATA_ENC_KEY_LEN           "EncryptionKeyLength"
#define DATABASE_BASIC_DATA_KEY_DERIVATION_FUNC   "KeyDerivationFunction"
#define DATABASE_BASIC_DATA_KEY_DERIVATION_ROUNDS "KeyDerivationRounds"
#define DATABASE_BASIC_DATA_KEY_DERIVATION_SALT   "KeyDerivationSalt"
#define DATABASE_BASIC_DATA_PASSWORD_HASH         "PasswordHash"

#define DB_SETTINGS_PADDING_SEPARATOR             '\0'
#define DB_SETTINGS_NAME_CLEAR_CLIPBOARD_TIME     "TimeToClearClipboard"
#define DB_SETTINGS_NAME_INACTIVE_BLOCK_TIME      "TimeInactiveToBlock"
#define DB_SETTINGS_NAME_BLOCK_WHEN_LOCKED        "BlockWhenLocked"
#define DB_SETTINGS_NAME_BLOCK_WHEN_MINIMIZED     "BlockWhenMinimized"
#define DB_SETTINGS_NAME_RANDOM_PADDING_INTERVAL  "RandomEntryPaddingInterval"
#define DEFAULT_DATABASE_SETTINGS                 {{ DB_SETTINGS_NAME_CLEAR_CLIPBOARD_TIME, "10" }, { DB_SETTINGS_NAME_INACTIVE_BLOCK_TIME, "3600" }, { DB_SETTINGS_NAME_BLOCK_WHEN_LOCKED, "0" }, { DB_SETTINGS_NAME_BLOCK_WHEN_MINIMIZED, "0" }, { DB_SETTINGS_NAME_RANDOM_PADDING_INTERVAL, "0-0" }}

#define DATABASE_ENTRY_TYPE_UNSPECIFIED           0
#define DATABASE_ENTRY_TYPE_CREDENTIAL            1
#define DATABASE_ENTRY_TYPE_CRYPTOCURRENCY        2
#define DATABASE_ENTRY_TYPE_API_KEY               3

#endif // CONSTANTS_H
