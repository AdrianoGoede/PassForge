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

#define DEFAULT_DATABASE_SETTINGS                 {{ "Language", "en-US" }, { "TimeToClearClipboard", "10" }, { "TimeInactiveToBlock", "3600" }, { "BlockWhenLocked", "false" }, { "BlockWhenMinimized", "false" }, { "EnableRandomEntryPadding", "false" }, { "EntryPaddingMin", "0" }, { "EntryPaddingMax", "100" }}

#define DATABASE_ENTRY_TYPE_UNSPECIFIED           0
#define DATABASE_ENTRY_TYPE_CREDENTIAL            1
#define DATABASE_ENTRY_TYPE_CRYPTOCURRENCY        2
#define DATABASE_ENTRY_TYPE_API_KEY               3

#endif // CONSTANTS_H
