#include "PasswordGenerator.h"
#include "Forms/ui_PasswordGenerator.h"
#include "../Configs/Constants.h"
#include "../Configs/Configs.h"
#include "../Crypto/Crypto.h"
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QTextStream>
#include <QClipboard>
#include <QString>
#include <QFile>
#include <QList>
#include <QDir>
#include <QSet>

PasswordGenerator::PasswordGenerator(QWidget *parent) : QDialog(parent), ui(new Ui::PasswordGenerator)
{
    ui->setupUi(this);
    setHashingAlgorithms();
    setDefaultWordlists();
    connect(ui->PasswdGenPushButton, &QPushButton::clicked, this, &PasswordGenerator::generatePassword);
    connect(ui->PasswdCopyPushButton, &QPushButton::clicked, this, &PasswordGenerator::copyPasswordToClipboard);
    connect(ui->PassphraseWordlistsAddButton, &QPushButton::clicked, this, &PasswordGenerator::addWordlist);
    connect(ui->PassphraseWordlistsRemoveButton, &QPushButton::clicked, this, &PasswordGenerator::removeWordlist);
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
    QString selectedChars = this->getSelectedCharaters();
    if (selectedChars.isEmpty()) return QString();

    size_t newPasswordLength = ui->PasswdLengthSpinBox->value();
    uint32_t randomValues[newPasswordLength];
    Crypto::getRandomUnsignedIntegers(randomValues, newPasswordLength, 0, (selectedChars.length() - 1));

    char newPassword[newPasswordLength + 1];
    for (uint32_t i = 0; i < newPasswordLength; i++)
        newPassword[i] = (selectedChars.at(randomValues[i]).toLatin1());
    newPassword[newPasswordLength] = '\0';
    QString result(newPassword);

    Crypto::wipeMemory(newPassword, sizeof(newPassword));
    Crypto::wipeMemory(randomValues, sizeof(randomValues));

    return result;
}

QString PasswordGenerator::generatePassphrase()
{
    this->loadWordlist();

    size_t newPasswordLength = ui->PassphraseLengthSpinBox->value();
    uint32_t randomValues[newPasswordLength];
    Crypto::getRandomUnsignedIntegers(randomValues, newPasswordLength, 0, (this->wordlist.size() - 1));

    QByteArray separator(!ui->PassphraseSeparatorLineEdit->text().isEmpty() ? ui->PassphraseSeparatorLineEdit->text().toUtf8() : " ");
    QByteArray buffer;
    for (uint32_t i = 0; i < newPasswordLength; i++) {
        if (i > 0) buffer.append(separator);
        QByteArray word = this->wordlist.at(randomValues[i]).toUtf8();
        switch (ui->PassphraseWordCaseComboBox->currentIndex()) {
            case 0: buffer.append(word.toLower()); break;
            case 1: buffer.append(word.toUpper()); break;
            case 2: {
                buffer.append(word.left(1).toUpper());
                buffer.append(word.mid(1).toLower());
            }; break;
        }
        Crypto::wipeMemory(word.data(), word.length());
    }
    QString result = QString::fromUtf8(buffer);

    Crypto::wipeMemory(buffer.data(), buffer.length());
    Crypto::wipeMemory(randomValues, sizeof(randomValues));

    return result;
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

void PasswordGenerator::addWordlist()
{
    QFileDialog dialog(this, "Wordlist File");
    dialog.setFilter(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries);
    if (dialog.exec() != QDialog::DialogCode::Accepted) return;
    ui->PassphraseWordlistsListWidget->addItem(dialog.selectedFiles().at(0));
    this->reloadWordlists = true;
}

void PasswordGenerator::removeWordlist()
{
    QList<QListWidgetItem*> selectedItems = ui->PassphraseWordlistsListWidget->selectedItems();
    if (selectedItems.empty()) return;

    QMessageBox::Button userChoice = QMessageBox::question(
        this,
        QString(),
        "Are you sure you want to remove the selected wordlists",
        (QMessageBox::Button::Yes | QMessageBox::Button::No)
    );

    if (userChoice != QMessageBox::Button::Yes) return;
    for (QListWidgetItem* item : selectedItems)
        if (item) delete item;
    this->reloadWordlists = true;
}

void PasswordGenerator::setHashingAlgorithms()
{
    std::vector<QString> algorithms({ SUPPORTED_HASHING_ALGORITHMS });
    for (const QString& algorithm : algorithms)
        ui->HashAlgorithmComboBox->addItem(algorithm);
}

void PasswordGenerator::setDefaultWordlists()
{
    QDir directory(WORDLISTS_RESOURCES_DIRECTORY);
    QStringList files = directory.entryList(QDir::Filter::Files);
    for (const QString& file : files)
        ui->PassphraseWordlistsListWidget->addItem(QString("%1/%2").arg(WORDLISTS_RESOURCES_DIRECTORY, file));
}

void PasswordGenerator::loadWordlist()
{
    if (!this->reloadWordlists) return;

    QSet<QString> words;
    for (size_t i = 0; i < ui->PassphraseWordlistsListWidget->count(); i++) {
        QFile file(ui->PassphraseWordlistsListWidget->item(i)->text());
        if (!file.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text)) continue;
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            words.insert((line.contains('\t') ? line.split('\t').last() : line).trimmed().toLower());
        }
    }

    this->wordlist = QVector<QString>(words.cbegin(), words.cend());
    this->reloadWordlists = false;
}

QString PasswordGenerator::getSelectedCharaters() const
{
    QByteArray buffer;
    buffer.append(ui->PasswdUpperCaseCharsCheckBox->checkState() == Qt::CheckState::Checked ? UPPER_CASE_CHARACTERS : "");
    buffer.append(ui->PasswdLowerCaseCharsCheckBox->checkState() == Qt::CheckState::Checked ? LOWER_CASE_CHARACTERS : "");
    buffer.append(ui->PasswdNumbersCheckBox->checkState() == Qt::CheckState::Checked ? NUMBER_CHARACTERS : "");
    buffer.append(ui->PasswdLogogramCharsCheckBox->checkState() == Qt::CheckState::Checked ? LOGOGRAM_CHARACTERS : "");
    buffer.append(ui->PasswdPunctuationCharsCheckBox->checkState() == Qt::CheckState::Checked ? PUNCTUATION_CHARACTERS : "");
    buffer.append(ui->PasswdDahesSlashesCheckBox->checkState() == Qt::CheckState::Checked ? DASH_SLASH_CHARACTERS : "");
    buffer.append(ui->PasswdMathSymbolsCheckBox->checkState() == Qt::CheckState::Checked ? MATH_SYMBOL_CHARACTERS : "");
    buffer.append(ui->PasswdBracesCheckBox->checkState() == Qt::CheckState::Checked ? BRACE_CHARACTERS : "");
    buffer.append(ui->PasswdQuoteCharsCheckBox->checkState() == Qt::CheckState::Checked ? QUOTE_CHARACTERS : "");
    buffer.append(ui->PasswdCustomCharsLineEdit->text().toUtf8());
    return QString::fromUtf8(buffer);
}
