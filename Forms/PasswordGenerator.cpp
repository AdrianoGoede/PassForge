#include "PasswordGenerator.h"
#include "Forms/ui_PasswordGenerator.h"
#include "../Configs/Constants.h"
#include "../Configs/Configs.h"
#include "../Crypto/Crypto.h"
#include <QClipboard>
#include <QString>
#include <sstream>

PasswordGenerator::PasswordGenerator(QWidget *parent) : QDialog(parent), ui(new Ui::PasswordGenerator)
{
    ui->setupUi(this);
    this->setHashingAlgorithms();
    connect(ui->PasswdGenPushButton, &QPushButton::clicked, this, &PasswordGenerator::generatePassword);
    connect(ui->PasswdCopyPushButton, &QPushButton::clicked, this, &PasswordGenerator::copyPasswordToClipboard);
}

PasswordGenerator::~PasswordGenerator() { delete ui; }

void PasswordGenerator::generatePassword()
{
    switch (ui->OptionsTabWidget->currentIndex()) {
        case 0: ui->PasswordLineEdit->setText(this->generateRandomPassword()); break;
        case 1: ui->PasswordLineEdit->setText(this->generatePassphrase()); break;
        case 2: ui->PasswordLineEdit->setText(this->generateHash()); break;
    }
}

QString PasswordGenerator::generateRandomPassword() const
{
    std::string selectedChars = this->getSelectedCharaters();
    if (selectedChars.empty()) return QString();

    size_t newPasswordLength = ui->PasswdLengthSpinBox->value();
    uint32_t randomValues[newPasswordLength];
    Crypto::getRandomUnsignedIntegers(randomValues, newPasswordLength, 0, (selectedChars.length() - 1));

    char newPassword[newPasswordLength + 1];
    for (uint32_t i = 0; i < newPasswordLength; i++)
        newPassword[i] = (selectedChars.at(randomValues[i]));
    newPassword[newPasswordLength] = '\0';
    QString result(newPassword);

    Crypto::wipeMemory(newPassword, sizeof(newPassword));
    Crypto::wipeMemory(randomValues, sizeof(randomValues));

    return result;
}

QString PasswordGenerator::generatePassphrase() const
{
    return QString(); // TO DO!
}

QString PasswordGenerator::generateHash() const
{
    QString plainText = ui->HashPlainTextEdit->toPlainText();
    std::string hash = Crypto::getHash(plainText.toStdString(), ui->HashAlgorithmComboBox->currentText().toStdString().c_str());
    return QString(hash.c_str());
}

void PasswordGenerator::copyPasswordToClipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    if (clipboard)
        clipboard->setText(ui->PasswordLineEdit->text());
}

void PasswordGenerator::setHashingAlgorithms()
{
    std::vector<QString> algorithms({ SUPPORTED_HASHING_ALGORITHMS });
    for (const QString& algorithm : algorithms)
        ui->HashAlgorithmComboBox->addItem(algorithm);
}

std::string PasswordGenerator::getSelectedCharaters() const
{
    std::stringstream stream;
    stream << (ui->PasswdUpperCaseCharsCheckBox->checkState() == Qt::CheckState::Checked ? UPPER_CASE_CHARACTERS : "");
    stream << (ui->PasswdLowerCaseCharsCheckBox->checkState() == Qt::CheckState::Checked ? LOWER_CASE_CHARACTERS : "");
    stream << (ui->PasswdNumbersCheckBox->checkState() == Qt::CheckState::Checked ? NUMBER_CHARACTERS : "");
    stream << (ui->PasswdLogogramCharsCheckBox->checkState() == Qt::CheckState::Checked ? LOGOGRAM_CHARACTERS : "");
    stream << (ui->PasswdPunctuationCharsCheckBox->checkState() == Qt::CheckState::Checked ? PUNCTUATION_CHARACTERS : "");
    stream << (ui->PasswdDahesSlashesCheckBox->checkState() == Qt::CheckState::Checked ? DASH_SLASH_CHARACTERS : "");
    stream << (ui->PasswdMathSymbolsCheckBox->checkState() == Qt::CheckState::Checked ? MATH_SYMBOL_CHARACTERS : "");
    stream << (ui->PasswdBracesCheckBox->checkState() == Qt::CheckState::Checked ? BRACE_CHARACTERS : "");
    stream << (ui->PasswdQuoteCharsCheckBox->checkState() == Qt::CheckState::Checked ? QUOTE_CHARACTERS : "");
    stream << ui->PasswdCustomCharsLineEdit->text().toStdString();
    return stream.str();
}
