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
    explicit DatabaseSettings(QWidget *parent = nullptr, const DatabaseHandler* dbHandler = nullptr);
    ~DatabaseSettings();

private slots:
    void accept() override;

private:
    Ui::DatabaseSettings *ui;
    const DatabaseHandler* dbHandler;
    bool timeToClearClipboardChanged = false;
    bool timeToBlockChanged = false;
    bool blockWhenMachineLockedChanged = false;
    bool blockWhenMinimizedChanged = false;
    bool randomEntryPaddingChanged = false;
};

#endif // DATABASESETTINGS_H
