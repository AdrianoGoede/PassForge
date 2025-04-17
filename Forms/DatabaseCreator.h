#ifndef DATABASECREATOR_H
#define DATABASECREATOR_H

#include "../Database/DatabaseHandler.h"
#include <QDialog>

namespace Ui {
class DatabaseCreator;
}

class DatabaseCreator : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseCreator(QWidget *parent = nullptr);
    ~DatabaseCreator();
    DatabaseHandler* getDatabaseHandler();

private slots:
    void selectFilePath();
    void setOkButtonEnabled();

private:
    Ui::DatabaseCreator *ui;

    bool validateCurrentState();
    void setEncryptionAlgorithms();
    void setEncryptionAlgorithmKeySettings();
    void setKeyDerivationFunctions();
};

#endif // DATABASECREATOR_H
