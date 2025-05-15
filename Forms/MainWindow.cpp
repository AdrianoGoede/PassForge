#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DatabaseCreator.h"
#include "DatabaseSettings.h"
#include "PasswordGenerator.h"
#include "ApiKeyEntryEditor.h"
#include "../Configs/Configs.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>

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
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CreateNewDatabase()
{
    try {
        DatabaseCreator creator(this);
        if (creator.exec() != QDialog::Accepted) return;

        this->databaseHandler.reset(creator.getDatabaseHandler());
        this->loadDatabase();
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
        this->loadDatabase();
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
            this->loadDatabase();
        }
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenNewCryptocurrency()
{

}

void MainWindow::QuitApplication() { QApplication::closeAllWindows(); }

void MainWindow::loadDatabase()
{
    QStandardItemModel* directoryTreeModel = new QStandardItemModel(this);
    ui->DirectoryStructureTreeView->setModel(directoryTreeModel);
    QStandardItem* currentNode = new QStandardItem("Root");
    directoryTreeModel->invisibleRootItem()->appendRow(currentNode);

    if (!this->databaseHandler) return;
    for (const DatabaseEntry& dbEntry : this->databaseHandler->getEntryHeaders()) {
        currentNode = directoryTreeModel->invisibleRootItem()->child(0);
        for (const QByteArray& directoryPart : dbEntry.getPath().split('/'))
            if (!directoryPart.isEmpty())
                currentNode = this->addDirectoryEntry(currentNode, directoryPart.trimmed());
    }
}

QStandardItem* MainWindow::addDirectoryEntry(QStandardItem* node, const QByteArray& folderName)
{
    for (size_t i = 0; i < node->rowCount(); i++)
        if (!node || node->child(i)->text().toUtf8() == folderName) return node->child(i);
    QStandardItem* child = new QStandardItem(folderName);
    node->appendRow(child);
    return child;
}
