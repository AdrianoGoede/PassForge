#ifndef DATABASECREATOR_H
#define DATABASECREATOR_H

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

private:
    Ui::DatabaseCreator *ui;

    void setEncryptionAlgorithms();
    void setEncryptionAlgorithmKeySettings();
    void setKeyDerivationFunctions();
};

#endif // DATABASECREATOR_H
