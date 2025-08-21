#include "DatabaseSettings.h"
#include "ui_DatabaseSettings.h"
#include "../Configs/Constants.h"
#include <QMessageBox>
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
    if (randomPaddingInterval.size() != 2) throw std::runtime_error("Random padding interval setting is not valid");
    ui->EnableEntryPaddingCheckBox->setChecked(randomPaddingInterval.size() == 2 && randomPaddingInterval.at(0) != "0" && randomPaddingInterval.at(1) != "0");
    ui->RandomEntryPaddingMinSpinBox->setValue(randomPaddingInterval.size() > 0 ? randomPaddingInterval.at(0).toInt() : 0);
    ui->RandomEntryPaddingMinSpinBox->setEnabled(randomPaddingInterval.at(0) != "0" && randomPaddingInterval.at(1) != "0");
    ui->RandomEntryPaddingMaxSpinBox->setValue(randomPaddingInterval.size() > 1 ? randomPaddingInterval.at(1).toInt() : ui->RandomEntryPaddingMinSpinBox->value());
    ui->RandomEntryPaddingMaxSpinBox->setEnabled(randomPaddingInterval.at(0) != "0" && randomPaddingInterval.at(1) != "0");

    connect(ui->EnableEntryPaddingCheckBox, &QCheckBox::clicked, ui->RandomEntryPaddingMinSpinBox, &QSpinBox::setEnabled);
    connect(ui->EnableEntryPaddingCheckBox, &QCheckBox::clicked, ui->RandomEntryPaddingMaxSpinBox, &QSpinBox::setEnabled);
    connect(ui->TimeToClearClipboardSpinBox, &QSpinBox::valueChanged, this, [this]() { this->timeToClearClipboardChanged = true; });
    connect(ui->TimeToBlockSpinBox, &QSpinBox::valueChanged, this, [this]() { this->timeToBlockChanged = true; });
    connect(ui->BlockWhenMachineLockedCheckBox, &QCheckBox::clicked, this, [this]() { this->blockWhenMachineLockedChanged = true; });
    connect(ui->BlockWhenMinimizedCheckBox, &QCheckBox::clicked, this, [this]() { this->blockWhenMinimizedChanged = true; });
    connect(ui->EnableEntryPaddingCheckBox, &QCheckBox::clicked, this, [this]() { this->randomEntryPaddingChanged = true; });
    connect(ui->RandomEntryPaddingMinSpinBox, &QSpinBox::valueChanged, this, [this]() { this->randomEntryPaddingChanged = true; });
    connect(ui->RandomEntryPaddingMaxSpinBox, &QSpinBox::valueChanged, this, [this]() { this->randomEntryPaddingChanged = true; });
    connect(ui->SavePushButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->CancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

DatabaseSettings::~DatabaseSettings() { delete ui; }

void DatabaseSettings::accept()
{
    SecureQByteArray value;

    if (this->timeToClearClipboardChanged) {
        try {
            value.setNum(ui->TimeToClearClipboardSpinBox->value());
            this->dbHandler->saveDatabaseSetting(DB_SETTINGS_NAME_CLEAR_CLIPBOARD_TIME, value);
        } catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
    }

    if (this->timeToBlockChanged) {
        try {
            value.setNum(ui->TimeToBlockSpinBox->value());
            this->dbHandler->saveDatabaseSetting(DB_SETTINGS_NAME_INACTIVE_BLOCK_TIME, value);
        } catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
    }

    if (this->blockWhenMachineLockedChanged) {
        try {
            value.setNum(ui->BlockWhenMachineLockedCheckBox->isChecked());
            this->dbHandler->saveDatabaseSetting(DB_SETTINGS_NAME_BLOCK_WHEN_LOCKED, value);
        } catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
    }

    if (this->blockWhenMinimizedChanged) {
        try {
            value.setNum(ui->BlockWhenMinimizedCheckBox->isChecked());
            this->dbHandler->saveDatabaseSetting(DB_SETTINGS_NAME_BLOCK_WHEN_MINIMIZED, value);
        } catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
    }

    if (this->randomEntryPaddingChanged)
        try {
            this->dbHandler->saveEntryPaddingSetting(
                (ui->EnableEntryPaddingCheckBox->isChecked() ? ui->RandomEntryPaddingMinSpinBox->value() : 0),
                (ui->EnableEntryPaddingCheckBox->isChecked() ? ui->RandomEntryPaddingMaxSpinBox->value() : 0)
            );
        } catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }

    QDialog::accept();
}
