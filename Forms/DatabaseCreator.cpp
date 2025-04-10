#include "DatabaseCreator.h"
#include "ui_DatabaseCreator.h"

#include "../Configs/Configs.h"
#include <QStringList>
#include <QString>

DatabaseCreator::DatabaseCreator(QWidget *parent) : QDialog(parent), ui(new Ui::DatabaseCreator)
{
    ui->setupUi(this);
    this->setEncryptionAlgorithms();
    this->setEncryptionAlgorithmKeySettings();
    this->setKeyDerivationFunctions();

    connect(ui->DatabaseEncryptionAlgorithmComboBox, &QComboBox::currentIndexChanged, this, &DatabaseCreator::setEncryptionAlgorithmKeySettings);
    connect(ui->OkPushButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->CancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

DatabaseCreator::~DatabaseCreator() { delete ui; }

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
