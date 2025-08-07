#include "DatabaseSettings.h"
#include "ui_DatabaseSettings.h"
#include <QMap>
#include <QString>

DatabaseSettings::DatabaseSettings(QWidget *parent, const DatabaseHandler* dbHandler) : QDialog(parent), dbHandler(dbHandler), ui(new Ui::DatabaseSettings)
{
    if (!this->dbHandler) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);

    ui->TimeToClearClipboardSpinBox->setValue(this->dbHandler->getDatabaseSetting("TimeToClearClipboard").toInt());
    ui->TimeToBlockSpinBox->setValue(this->dbHandler->getDatabaseSetting("TimeInactiveToBlock").toInt());
    ui->BlockWhenMachineLockedCheckBox->setChecked(this->dbHandler->getDatabaseSetting("BlockWhenLocked").toInt());
    ui->BlockWhenMinimizedCheckBox->setChecked(this->dbHandler->getDatabaseSetting("BlockWhenMinimized").toInt());
    ui->EnableEntryPaddingCheckBox->setChecked(this->dbHandler->getDatabaseSetting("EnableRandomEntryPadding").toInt());
    ui->RandomEntryPaddingMinSpinBox->setValue(this->dbHandler->getDatabaseSetting("EntryPaddingMin").toInt());
    ui->RandomEntryPaddingMaxSpinBox->setValue(this->dbHandler->getDatabaseSetting("EntryPaddingMax").toInt());

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
