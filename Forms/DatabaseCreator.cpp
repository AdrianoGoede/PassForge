#include "DatabaseCreator.h"
#include "ui_DatabaseCreator.h"
#include "../Configs/Configs.h"
#include "../Crypto/Crypto.h"
#include <QStringList>
#include <QFileDialog>
#include <QByteArray>
#include <QString>

DatabaseCreator::DatabaseCreator(QWidget *parent) : QDialog(parent), ui(new Ui::DatabaseCreator)
{
    ui->setupUi(this);
    this->setEncryptionAlgorithms();
    this->setEncryptionAlgorithmKeySettings();
    this->setKeyDerivationFunctions();

    connect(ui->DatabaseNameSelectPushButton, &QPushButton::clicked, this, &DatabaseCreator::selectFilePath);
    connect(ui->DatabaseEncryptionAlgorithmComboBox, &QComboBox::currentIndexChanged, this, &DatabaseCreator::setEncryptionAlgorithmKeySettings);
    connect(ui->DatabaseNameLineEdit, &QLineEdit::textChanged, this, &DatabaseCreator::setOkButtonEnabled);
    connect(ui->DatabasePasswordLineEdit, &QLineEdit::textChanged, this, &DatabaseCreator::setOkButtonEnabled);
    connect(ui->DatabasePasswordRepeatLineEdit, &QLineEdit::textChanged, this, &DatabaseCreator::setOkButtonEnabled);
    connect(ui->OkPushButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->CancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

DatabaseCreator::~DatabaseCreator() { delete ui; }

DatabaseHandler* DatabaseCreator::getDatabaseHandler()
{
    if (!this->validateCurrentState()) return nullptr; // TO DO!

    DatabaseHandlerOptions options {
        ui->DatabaseDescriptionLineEdit->text().trimmed(),
        ui->DatabaseEncryptionAlgorithmComboBox->currentText().trimmed(),
        (uint16_t)ui->DatabaseEncryptionKeyLengthComboBox->currentText().trimmed().toInt(),
        ui->DatabaseKeyDerivationFunctionComboBox->currentText().trimmed()
    };
    return new DatabaseHandler(ui->DatabaseNameLineEdit->text(), ui->DatabasePasswordLineEdit->text().toUtf8(), &options);
}

void DatabaseCreator::selectFilePath() { ui->DatabaseNameLineEdit->setText(QFileDialog::getSaveFileName(this, "Select database path and name", QString(), DATABASE_FILE_FILTER)); }

void DatabaseCreator::setOkButtonEnabled() { ui->OkPushButton->setEnabled(this->validateCurrentState()); }

bool DatabaseCreator::validateCurrentState()
{
    QString dbName = ui->DatabaseNameLineEdit->text();
    QString passwd1 = ui->DatabasePasswordLineEdit->text();
    QString passwd2 = ui->DatabasePasswordRepeatLineEdit->text();

    bool result = (!dbName.isEmpty() && passwd1.length() >= DATABASE_MIN_PASSWORD_LENGTH && passwd1 == passwd2);

    Crypto::wipeMemory(passwd1.data(), (sizeof(QChar) * passwd1.length()));
    Crypto::wipeMemory(passwd2.data(), (sizeof(QChar) * passwd2.length()));

    return result;
}

void DatabaseCreator::setEncryptionAlgorithms()
{
    for (const QString& algorithm : QStringList({ SUPPORTED_ENCRYPTION_ALGORITHMS }))
        ui->DatabaseEncryptionAlgorithmComboBox->addItem(algorithm.trimmed());
}

void DatabaseCreator::setEncryptionAlgorithmKeySettings()
{
    ui->DatabaseEncryptionKeyLengthComboBox->clear();

    QStringList settings;
    if (ui->DatabaseEncryptionAlgorithmComboBox->currentText() == "ChaCha")
        settings.append(QStringList({ SUPPORTED_STREAM_CYPHER_KEY_SETTINGS }));
    else
        settings.append(QStringList({ SUPPORTED_BLOCK_CYPHER_KEY_SETTINGS }));

    for (const QString& setting : settings)
        ui->DatabaseEncryptionKeyLengthComboBox->addItem(setting.trimmed());
}

void DatabaseCreator::setKeyDerivationFunctions()
{
    for (const QString& function : QStringList({ SUPPORTED_KEY_DERIVATION_FUNCTIONS }))
        ui->DatabaseKeyDerivationFunctionComboBox->addItem(function.trimmed());
}
