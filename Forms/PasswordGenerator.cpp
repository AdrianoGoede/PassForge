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
    connect(ui->PasswordLineEdit, &QLineEdit::textChanged, this, &PasswordGenerator::validatePassword);
    connect(ui->OkPushButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->CancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

PasswordGenerator::~PasswordGenerator() { delete ui; }

QString PasswordGenerator::getValue() const{ return ui->PasswordLineEdit->text(); }

void PasswordGenerator::generatePassword()
{
    QByteArray value;
    switch (ui->OptionsTabWidget->currentIndex()) {
        case 0: value.append(this->generateRandomPassword()); break;
        case 1: value.append(this->generatePassphrase()); break;
        case 2: value.append(this->generateHash()); break;
        default: return;
    }
    ui->PasswordLineEdit->setText(value);
    Crypto::wipeMemory(value.data(), value.length());
}

void PasswordGenerator::validatePassword()
{
    QByteArray passwd = ui->PasswordLineEdit->text().toUtf8();
    ui->OkPushButton->setEnabled(!passwd.isEmpty());
    Crypto::wipeMemory(passwd.data(), passwd.length());
}

QByteArray PasswordGenerator::generateRandomPassword() const
{
    QString selectedChars = this->getSelectedCharaters();
    if (selectedChars.isEmpty()) return QByteArray();

    size_t newPasswordLength = ui->PasswdLengthSpinBox->value();
    uint32_t randomValues[newPasswordLength];
    Crypto::getRandomUnsignedIntegers(randomValues, newPasswordLength, 0, (selectedChars.length() - 1));

    QByteArray result;
    for (uint32_t i = 0; i < newPasswordLength; i++)
        result.append((selectedChars.at(randomValues[i]).toLatin1()));

    Crypto::wipeMemory(randomValues, sizeof(randomValues));
    return result;
}

QByteArray PasswordGenerator::generatePassphrase()
{
    this->loadWordlist();

    size_t newPasswordLength = ui->PassphraseLengthSpinBox->value();
    uint32_t randomValues[newPasswordLength];
    Crypto::getRandomUnsignedIntegers(randomValues, newPasswordLength, 0, (this->wordlist.size() - 1));

    QByteArray separator(!ui->PassphraseSeparatorLineEdit->text().isEmpty() ? ui->PassphraseSeparatorLineEdit->text().toUtf8() : " ");
    QByteArray result;
    for (uint32_t i = 0; i < newPasswordLength; i++) {
        if (i > 0) result.append(separator);
        QByteArray word = this->wordlist.at(randomValues[i]).toUtf8();
        switch (ui->PassphraseWordCaseComboBox->currentIndex()) {
            case 0: result.append(word.toLower()); break;
            case 1: result.append(word.toUpper()); break;
            case 2: {
                result.append(word.left(1).toUpper());
                result.append(word.mid(1).toLower());
            }; break;
        }
        Crypto::wipeMemory(word.data(), (sizeof(char) * word.length()));
    }

    Crypto::wipeMemory(randomValues, sizeof(randomValues));
    return result;
}

QByteArray PasswordGenerator::generateHash() const
{
    QByteArray plainText(ui->HashPlainTextEdit->toPlainText().toUtf8());
    if (plainText.isEmpty()) return QByteArray();
    std::string hash = Crypto::getHash(plainText.toStdString(), ui->HashAlgorithmComboBox->currentText().toStdString().c_str());
    Crypto::wipeMemory(plainText.data(), (sizeof(char) * plainText.length()));
    return QByteArray(hash.c_str());
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
    for (const QString& algorithm : QStringList({ SUPPORTED_HASHING_ALGORITHMS }))
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
