#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Database/DatabaseHandler.h"
#include "Models/SecureTreeModel.h"
#include "Models/SecureDatabaseEntryModel.h"
#include <QStandardItem>

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
    void OpenNewCredentialWindow();
    void OpenNewApiKeyWindow();
    void OpenNewCryptocurrencyWindow();
    void DeleteEntry();
    void OpenEntryManegementWindow(const QModelIndex& index);
    void LoadDatabaseEntries(const QModelIndex& index);
    void QuitApplication();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<DatabaseHandler> databaseHandler = nullptr;
    std::unique_ptr<SecureTreeModel> directoryTreeModel = nullptr;
    std::unique_ptr<SecureDatabaseEntryListModel> entryListModel = nullptr;

    void loadDirectoryStructure();
    SecureQByteArray getSelectedPath(const QModelIndex& index) const;
    void setDatabaseControlsEnabled(bool enabled);
    void closeDatabase();
};

#endif // MAINWINDOW_H
