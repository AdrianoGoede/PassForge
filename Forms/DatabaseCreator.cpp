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
    connect(ui->DatabaseKeyDerivationFunctionComboBox, &QComboBox::currentTextChanged, this, &DatabaseCreator::setIterationSelector);
    connect(ui->DatabasePasswordLineEdit, &QLineEdit::textChanged, this, &DatabaseCreator::setOkButtonEnabled);
    connect(ui->DatabasePasswordRepeatLineEdit, &QLineEdit::textChanged, this, &DatabaseCreator::setOkButtonEnabled);
    connect(ui->OkPushButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->CancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

DatabaseCreator::~DatabaseCreator() { delete ui; }

DatabaseHandler* DatabaseCreator::getDatabaseHandler()
{
    this->validateCurrentState();

    DatabaseHandlerOptions options {
        ui->DatabaseDescriptionLineEdit->text().trimmed().toStdString(),
        ui->DatabaseEncryptionAlgorithmComboBox->currentText().trimmed().toStdString(),
        (uint16_t)ui->DatabaseEncryptionKeyLengthComboBox->currentText().trimmed().toInt(),
        ui->DatabaseKeyDerivationFunctionComboBox->currentText().trimmed().toStdString(),
        (uint32_t)ui->DatabaseKeyDerivationTransformRoundsSpinBox->value()
    };
    return new DatabaseHandler(ui->DatabaseNameLineEdit->text(), ui->DatabasePasswordLineEdit->text().toUtf8(), &options);
}

void DatabaseCreator::selectFilePath() { ui->DatabaseNameLineEdit->setText(QFileDialog::getSaveFileName(this, "Select location", QString(), DATABASE_FILE_FILTER)); }

void DatabaseCreator::setIterationSelector(const QString& selected)
{
    int min, max;

    if (selected == "PBKDF2") {
        min = KEY_DERIVATION_PBKDF2_MIN_ITERATIONS;
        max = KEY_DERIVATION_PBKDF2_MAX_ITERATIONS;
    }
    else if (selected == "Scrypt") {
        min = KEY_DERIVATION_SCRYPT_MIN_ITERATIONS;
        max = KEY_DERIVATION_SCRYPT_MAX_ITERATIONS;
    }
    else if (selected == "Argon2id") {
        // TO DO!
    }
    else {
        min = 0;
        max = 0;
    }

    ui->DatabaseKeyDerivationTransformRoundsSpinBox->setMinimum(min);
    ui->DatabaseKeyDerivationTransformRoundsSpinBox->setMaximum(max);
}

void DatabaseCreator::setOkButtonEnabled()
{
    try {
        this->validateCurrentState();
        ui->OkPushButton->setEnabled(true);
    }
    catch (const std::runtime_error& error) { ui->OkPushButton->setEnabled(false); }
}

void DatabaseCreator::validateCurrentState()
{
    QString dbName = ui->DatabaseNameLineEdit->text();
    QByteArray passwd1 = ui->DatabasePasswordLineEdit->text().toUtf8();
    QByteArray passwd2 = ui->DatabasePasswordRepeatLineEdit->text().toUtf8();

    QString error;
    if (dbName.isEmpty())
        error += "Database name cannot be empty";
    else if (passwd1.length() < DATABASE_MIN_PASSWORD_LENGTH)
        error += QString("Password must be at least %1 characters long").arg(DATABASE_MIN_PASSWORD_LENGTH);
    else if (passwd1 != passwd2)
        error += "The passwords don't match";

    Crypto::wipeMemory(passwd1.data(), passwd1.length());
    Crypto::wipeMemory(passwd2.data(), passwd2.length());

    if (!error.isEmpty()) throw std::runtime_error(error.toStdString());
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
