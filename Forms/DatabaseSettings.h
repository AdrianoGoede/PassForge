#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include <QDialog>
#include "../Database/DatabaseHandler.h"

namespace Ui {
class DatabaseSettings;
}

class DatabaseSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseSettings(QWidget *parent = nullptr, DatabaseHandler* dbHandler = nullptr);
    ~DatabaseSettings();

private:
    Ui::DatabaseSettings *ui;
    DatabaseHandler* dbHandler;
};

#endif // DATABASESETTINGS_H
