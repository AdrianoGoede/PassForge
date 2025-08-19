#include "DatabaseSettings.h"
#include "ui_DatabaseSettings.h"
#include "../Configs/Constants.h"
#include <QMap>
#include <QString>
#include <QVector>

DatabaseSettings::DatabaseSettings(QWidget *parent, const DatabaseHandler* dbHandler) : QDialog(parent), dbHandler(dbHandler), ui(new Ui::DatabaseSettings)
{
    if (!this->dbHandler) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);

    ui->TimeToClearClipboardSpinBox->setValue(this->dbHandler->getDatabaseSetting(DB_SETTINGS_NAME_CLEAR_CLIPBOARD_TIME).toInt());
    ui->TimeToBlockSpinBox->setValue(this->dbHandler->getDatabaseSetting(DB_SETTINGS_NAME_INACTIVE_BLOCK_TIME).toInt());
    ui->BlockWhenMachineLockedCheckBox->setChecked(this->dbHandler->getDatabaseSetting(DB_SETTINGS_NAME_BLOCK_WHEN_LOCKED).toInt());
    ui->BlockWhenMinimizedCheckBox->setChecked(this->dbHandler->getDatabaseSetting(DB_SETTINGS_NAME_BLOCK_WHEN_MINIMIZED).toInt());

    QVector<SecureQByteArray> randomPaddingInterval = this->dbHandler->getDatabaseSetting(DB_SETTINGS_NAME_RANDOM_PADDING_INTERVAL).secureSplit('-');
    ui->EnableEntryPaddingCheckBox->setChecked(randomPaddingInterval.size() == 2 && randomPaddingInterval.at(0) != "0" && randomPaddingInterval.at(1) != "0");
    ui->RandomEntryPaddingMinSpinBox->setValue(randomPaddingInterval.size() > 0 ? randomPaddingInterval.at(0).toInt() : 0);
    ui->RandomEntryPaddingMaxSpinBox->setValue(randomPaddingInterval.size() > 1 ? randomPaddingInterval.at(1).toInt() : ui->RandomEntryPaddingMinSpinBox->value());

    connect(ui->EnableEntryPaddingCheckBox, &QCheckBox::clicked, ui->RandomEntryPaddingMinSpinBox, &QSpinBox::setEnabled);
    connect(ui->EnableEntryPaddingCheckBox, &QCheckBox::clicked, ui->RandomEntryPaddingMaxSpinBox, &QSpinBox::setEnabled);
    connect(ui->TimeToClearClipboardSpinBox, &QSpinBox::valueChanged, this, [this]() { this->timeToClearClipboardChanged = true; });
    connect(ui->TimeToBlockSpinBox, &QSpinBox::valueChanged, this, [this]() { this->timeToBlockChanged = true; });
    connect(ui->BlockWhenMachineLockedCheckBox, &QCheckBox::clicked, this, [this]() { this->blockWhenMachineLockedChanged = true; });
    connect(ui->BlockWhenMinimizedCheckBox, &QCheckBox::clicked, this, [this]() { this->blockWhenMinimizedChanged = true; });
    connect(ui->EnableEntryPaddingCheckBox, &QCheckBox::clicked, this, [this]() { this->enableEntryPaddingChanged = true; });
    connect(ui->RandomEntryPaddingMinSpinBox, &QSpinBox::valueChanged, this, [this]() { this->randomEntryPaddingMinChanged = true; });
    connect(ui->RandomEntryPaddingMaxSpinBox, &QSpinBox::valueChanged, this, [this]() { this->randomEntryPaddingMaxChanged = true; });
    connect(ui->SavePushButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->CancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

DatabaseSettings::~DatabaseSettings() { delete ui; }

void DatabaseSettings::accept()
{
    QDialog::accept();
}
