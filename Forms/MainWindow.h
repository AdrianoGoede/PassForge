#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Database/DatabaseHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void CreateNewDatabase();
    void OpenExistingDatabase();
    void OpenDatabaseSettings();
    void OpenPasswordGenerator();
    void QuitApplication();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<DatabaseHandler> databaseHandler = nullptr;

    void loadDatabase();
};

#endif // MAINWINDOW_H
