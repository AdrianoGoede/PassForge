#include "DatabaseSettings.h"
#include "ui_DatabaseSettings.h"
#include <QMap>
#include <QString>

DatabaseSettings::DatabaseSettings(QWidget *parent, const DatabaseHandler* dbHandler) : QDialog(parent), dbHandler(dbHandler), ui(new Ui::DatabaseSettings)
{
    if (!this->dbHandler) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);

    ui->TimeToClearClipboardSpinBox->setValue(this->dbHandler->getDatabaseSetting("TimeToClearClipboard").toInt());
    // ui->TimeToBlockSpinBox->setValue(this->dbHandler->getDatabaseSetting("TimeInactiveToBlock").toInt());
    // ui->BlockWhenMachineLockedCheckBox->setChecked(this->dbHandler->getDatabaseSetting("BlockWhenLocked").toInt());
    // ui->BlockWhenMinimizedCheckBox->setChecked(this->dbHandler->getDatabaseSetting("BlockWhenMinimized").toInt());
    // ui->EnableEntryPaddingCheckBox->setChecked(this->dbHandler->getDatabaseSetting("EnableRandomEntryPadding").toInt());
}

DatabaseSettings::~DatabaseSettings() { delete ui; }
