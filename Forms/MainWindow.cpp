#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DatabaseCreator.h"
#include "DatabaseSettings.h"
#include "PasswordGenerator.h"
#include "ApiKeyEntryEditor.h"
#include "../Configs/Configs.h"
#include "Models/SecureTreeModel.h"
#include "Models/SecureDatabaseEntryModel.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->CentralWidget->setLayout(ui->CentralGridLayout);

    QMenu* newEntryMenu = new QMenu(ui->AddEntryPushButton);
    newEntryMenu->addAction("Credential", this, &MainWindow::OpenNewCredentialWindow);
    newEntryMenu->addAction("Api Key", this, &MainWindow::OpenNewApiKeyWindow);
    newEntryMenu->addAction("Cryptocurrency", this, &MainWindow::OpenNewCryptocurrency);
    ui->AddEntryPushButton->setMenu(newEntryMenu);

    connect(ui->ActionDatabaseNew, &QAction::triggered, this, &MainWindow::CreateNewDatabase);
    connect(ui->ActionDatabaseOpen, &QAction::triggered, this, &MainWindow::OpenExistingDatabase);
    connect(ui->ActionDatabaseSettings, &QAction::triggered, this, &MainWindow::OpenDatabaseSettings);
    connect(ui->ActionToolsPasswordGenerator, &QAction::triggered, this, &MainWindow::OpenPasswordGenerator);
    connect(ui->ActionApplicationQuit, &QAction::triggered, this, &MainWindow::QuitApplication);
    connect(ui->DirectoryStructureTreeView, &QAbstractItemView::clicked, this, &MainWindow::LoadDatabaseEntries);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CreateNewDatabase()
{
    try {
        DatabaseCreator creator(this);
        if (creator.exec() != QDialog::Accepted) return;

        this->databaseHandler.reset(creator.getDatabaseHandler());
        this->LoadDirectoryStructure();
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenExistingDatabase()
{
    try {
        QString dbPath = QFileDialog::getOpenFileName(this, QString(), QString(), DATABASE_FILE_FILTER);
        if (dbPath.isEmpty()) return;

        bool ok;
        SecureQByteArray password(QInputDialog::getText(this, "Enter database password", "Password", QLineEdit::EchoMode::Password, "", &ok).toUtf8());
        if (!ok) return;
        if (password.isEmpty()) throw std::runtime_error("Database password cannot be empty");

        this->databaseHandler.reset(new DatabaseHandler(dbPath, password));
        this->LoadDirectoryStructure();
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenDatabaseSettings()
{
    DatabaseSettings settings(this);
    settings.exec();
}

void MainWindow::OpenPasswordGenerator()
{
    PasswordGenerator generator(this);
    generator.exec();
}

void MainWindow::OpenNewCredentialWindow()
{

}

void MainWindow::OpenNewApiKeyWindow()
{
    try {
        ApiKeyEntry dbEntry;
        if (ApiKeyEntryEditor(this, &dbEntry).exec() == QDialog::Accepted) {
            this->databaseHandler->saveDatabaseEntry(dbEntry);
            this->LoadDirectoryStructure();
        }
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenNewCryptocurrency()
{

}

void MainWindow::LoadDatabaseEntries(const QModelIndex& index)
{
    SecureDatabaseEntryListModel* entryListModel = new SecureDatabaseEntryListModel(this);
    ui->EntryListView->setModel(entryListModel);
    SecureQByteArray selectedPath = GetSelectedPath(index);
    for (const DatabaseEntry& entry : this->databaseHandler->getEntryHeaders(selectedPath))
        entryListModel->addItem(QModelIndex(), entry);
}

void MainWindow::QuitApplication() { QApplication::closeAllWindows(); }

void MainWindow::LoadDirectoryStructure()
{
    try {
        SecureTreeModel* directoryTreeModel = new SecureTreeModel(this);
        ui->DirectoryStructureTreeView->setModel(directoryTreeModel);
        if (!this->databaseHandler) return;

        for (const SecureQByteArray& entryPath : this->databaseHandler->getEntryPaths()) {
            QModelIndex parentIndex;

            for (SecureQByteArray& directoryPart : entryPath.secureSplit('/')) {
                if (directoryPart.isEmpty()) directoryPart = "Root";

                bool found = false;
                int rowCount = directoryTreeModel->rowCount();

                for (int row = 0; row < rowCount; row++) {
                    QModelIndex child = directoryTreeModel->index(row, 0, parentIndex);
                    if (directoryTreeModel->data(child).toByteArray() == directoryPart) {
                        parentIndex = child;
                        found = true;
                        break;
                    }
                }

                if (found) continue;
                directoryTreeModel->addItem(parentIndex, SecureQByteArray(directoryPart.data()));
                parentIndex = directoryTreeModel->index(directoryTreeModel->rowCount(parentIndex) - 1, 0, parentIndex);
            }
        }
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

SecureQByteArray MainWindow::GetSelectedPath(const QModelIndex& index) const
{
    if (!index.isValid()) return SecureQByteArray();
    const QModelIndex parent = index.parent();
    if (parent.isValid())
        return (this->GetSelectedPath(parent) + "/" + SecureQByteArray(index.data().toByteArray()));
    return SecureQByteArray();
}
