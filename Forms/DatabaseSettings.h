#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include <QDialog>

namespace Ui {
class DatabaseSettings;
}

class DatabaseSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseSettings(QWidget *parent = nullptr);
    ~DatabaseSettings();

private:
    Ui::DatabaseSettings *ui;
};

#endif // DATABASESETTINGS_H
