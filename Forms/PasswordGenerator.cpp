#include "PasswordGenerator.h"
#include "Forms/ui_PasswordGenerator.h"
#include "../Configs/Constants.h"
#include "../Crypto/Crypto.h"
#include <QClipboard>
#include <sstream>

PasswordGenerator::PasswordGenerator(QWidget *parent) : QDialog(parent), ui(new Ui::PasswordGenerator)
{
    ui->setupUi(this);
    connect(ui->PasswdGenPushButton, &QPushButton::clicked, this, &PasswordGenerator::generatePassword);
    connect(ui->PasswdCopyPushButton, &QPushButton::clicked, this, &PasswordGenerator::copyPasswordToClipboard);
}

PasswordGenerator::~PasswordGenerator() { delete ui; }

void PasswordGenerator::generatePassword()
{
    std::string selectedChars = this->getSelectedCharaters();
    if (selectedChars.empty()) return;

    uint32_t randomValues[ui->PasswdLengthSpinBox->value()];
    Crypto::getRandomUnsignedIntegers(randomValues, ui->PasswdLengthSpinBox->value(), 0, (selectedChars.length() - 1));

    std::stringstream stream;
    for (uint32_t i = 0; i < ui->PasswdLengthSpinBox->value(); i++)
        stream << selectedChars.at(randomValues[i]);
    std::memset(stream.str().data(), 0, (sizeof(char) * stream.str().size()));
    std::memset(randomValues, 0, (sizeof(uint32_t) * ui->PasswdLengthSpinBox->value()));
    ui->PasswordLineEdit->setText(stream.str().c_str());
}

void PasswordGenerator::copyPasswordToClipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    if (clipboard)
        clipboard->setText(ui->PasswordLineEdit->text());
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
