#include "DatabaseSettings.h"
#include "ui_DatabaseSettings.h"

DatabaseSettings::DatabaseSettings(QWidget *parent, DatabaseHandler* dbHandler) : QDialog(parent), dbHandler(dbHandler), ui(new Ui::DatabaseSettings)
{
    if (!this->dbHandler) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);
}

DatabaseSettings::~DatabaseSettings() { delete ui; }
