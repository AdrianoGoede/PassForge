#include "DatabaseSettings.h"
#include "ui_DatabaseSettings.h"

DatabaseSettings::DatabaseSettings(QWidget *parent) : QDialog(parent), ui(new Ui::DatabaseSettings)
{
    ui->setupUi(this);
}

DatabaseSettings::~DatabaseSettings() { delete ui; }
