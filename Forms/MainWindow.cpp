#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DatabaseCreator.h"
#include "DatabaseSettings.h"
#include "PasswordGenerator.h"
#include "ApiKeyEntryEditor.h"
#include "../Configs/Configs.h"
#include "../Configs/Constants.h"
#include "CryptocurrencyEntryEditor.h"
#include "CredentialEntryEditor.h"
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
    newEntryMenu->addAction("Cryptocurrency", this, &MainWindow::OpenNewCryptocurrencyWindow);
    ui->AddEntryPushButton->setMenu(newEntryMenu);

    connect(ui->ActionDatabaseNew, &QAction::triggered, this, &MainWindow::CreateNewDatabase);
    connect(ui->ActionDatabaseOpen, &QAction::triggered, this, &MainWindow::OpenExistingDatabase);
    connect(ui->ActionDatabaseSettings, &QAction::triggered, this, &MainWindow::OpenDatabaseSettings);
    connect(ui->ActionDatabaseClose, &QAction::triggered, this, &MainWindow::closeDatabase);
    connect(ui->ActionToolsPasswordGenerator, &QAction::triggered, this, &MainWindow::OpenPasswordGenerator);
    connect(ui->ActionApplicationQuit, &QAction::triggered, this, &MainWindow::QuitApplication);
    connect(ui->DirectoryStructureTreeView, &QAbstractItemView::clicked, this, &MainWindow::LoadDatabaseEntries);
    connect(ui->EntryListView, &QAbstractItemView::doubleClicked, this, &MainWindow::OpenEntryManegementWindow);
    connect(ui->DeleteEntryPushButton, &QAbstractButton::clicked, this, &MainWindow::DeleteEntry);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CreateNewDatabase()
{
    try {
        DatabaseCreator creator(this);
        if (creator.exec() != QDialog::Accepted) return;

        this->setDatabaseControlsEnabled(false);
        this->databaseHandler.reset(creator.getDatabaseHandler());
        this->loadDirectoryStructure();
        this->setDatabaseControlsEnabled(true);
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

        this->setDatabaseControlsEnabled(false);
        this->databaseHandler.reset(new DatabaseHandler(dbPath, password));
        this->loadDirectoryStructure();
        this->setDatabaseControlsEnabled(true);
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenDatabaseSettings()
{
    try {
        DatabaseSettings settings(this, this->databaseHandler.get());
        settings.exec();
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenPasswordGenerator()
{
    PasswordGenerator generator(this);
    generator.exec();
}

void MainWindow::OpenNewCredentialWindow()
{
    try {
        CredentialEntry dbEntry;
        if (CredentialEntryEditor(this, &dbEntry).exec() == QDialog::Accepted) {
            this->databaseHandler->saveDatabaseEntry(dbEntry);
            this->loadDirectoryStructure();
        }
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenNewApiKeyWindow()
{
    try {
        ApiKeyEntry dbEntry;
        if (ApiKeyEntryEditor(this, &dbEntry).exec() == QDialog::Accepted) {
            this->databaseHandler->saveDatabaseEntry(dbEntry);
            this->loadDirectoryStructure();
        }
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenNewCryptocurrencyWindow()
{
    try {
        CryptocurrencyEntry dbEntry;
        if (CryptocurrencyEntryEditor(this, &dbEntry).exec() == QDialog::Accepted) {
            this->databaseHandler->saveDatabaseEntry(dbEntry);
            this->loadDirectoryStructure();
        }
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::DeleteEntry()
{
    try {
        const DatabaseEntry& dbEntry = qobject_cast<SecureDatabaseEntryListModel*>(ui->EntryListView->model())->getDbEntry(ui->EntryListView->currentIndex());
        if (dbEntry.getEntryId() <= 0) return;
        bool sure = (QMessageBox::question(
            this,
            "Are you sure?",
            QString("Are you sure you want to delete entry %1").arg(dbEntry.getEntryId()),
            (QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No)
        ) == QMessageBox::StandardButton::Yes);
        if (!sure) return;

        this->databaseHandler->deleteDatabaseEntry(dbEntry.getEntryId());
        this->LoadDatabaseEntries(ui->DirectoryStructureTreeView->currentIndex());
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::OpenEntryManegementWindow(const QModelIndex& index)
{
    try {
        bool reloadDirectoryStructure = false;
        const DatabaseEntry& dbEntryHeader = qobject_cast<SecureDatabaseEntryListModel*>(ui->EntryListView->model())->getDbEntry(index);

        switch (dbEntryHeader.getEntryType()) {
            case DATABASE_ENTRY_TYPE_CREDENTIAL: {
                CredentialEntry dbEntry = this->databaseHandler->getCredentialEntry(dbEntryHeader);
                if (CredentialEntryEditor(this, &dbEntry).exec() == QDialog::Accepted) {
                    this->databaseHandler->saveDatabaseEntry(dbEntry);
                    reloadDirectoryStructure = dbEntry.getPath() != dbEntryHeader.getPath();
                }
                else return;
            } break;
            case DATABASE_ENTRY_TYPE_CRYPTOCURRENCY: {
                CryptocurrencyEntry dbEntry = this->databaseHandler->getCryptocurrencyEntry(dbEntryHeader);
                if (CryptocurrencyEntryEditor(this, &dbEntry).exec() == QDialog::Accepted) {
                    this->databaseHandler->saveDatabaseEntry(dbEntry);
                    reloadDirectoryStructure = dbEntry.getPath() != dbEntryHeader.getPath();
                }
                else return;
            } break;
            case DATABASE_ENTRY_TYPE_API_KEY: {
                ApiKeyEntry dbEntry = this->databaseHandler->getApiKeyEntry(dbEntryHeader);
                if (ApiKeyEntryEditor(this, &dbEntry).exec() == QDialog::Accepted) {
                    this->databaseHandler->saveDatabaseEntry(dbEntry);
                    reloadDirectoryStructure = dbEntry.getPath() != dbEntryHeader.getPath();
                }
                else return;
            } break;
            default: return;
        }

        if (reloadDirectoryStructure)
            this->loadDirectoryStructure();
        else
            this->LoadDatabaseEntries(ui->DirectoryStructureTreeView->currentIndex());
    }
    catch (const std::runtime_error& error) { QMessageBox::critical(this, "Error", error.what()); }
}

void MainWindow::LoadDatabaseEntries(const QModelIndex& index)
{
    this->entryListModel.reset(new SecureDatabaseEntryListModel(this));
    ui->EntryListView->setModel(this->entryListModel.get());
    SecureQByteArray selectedPath = getSelectedPath(index);
    for (const DatabaseEntry& entry : this->databaseHandler->getEntryHeaders(selectedPath))
        entryListModel->addItem(QModelIndex(), entry);
}

void MainWindow::QuitApplication() { QApplication::closeAllWindows(); }

void MainWindow::loadDirectoryStructure()
{
    try {
        this->directoryTreeModel.reset(new SecureTreeModel(this));
        ui->DirectoryStructureTreeView->setModel(this->directoryTreeModel.get());
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

SecureQByteArray MainWindow::getSelectedPath(const QModelIndex& index) const
{
    if (!index.isValid()) return SecureQByteArray();
    const QModelIndex parent = index.parent();
    if (parent.isValid())
        return (this->getSelectedPath(parent) + "/" + SecureQByteArray(index.data().toByteArray()));
    return SecureQByteArray();
}

void MainWindow::setDatabaseControlsEnabled(bool enabled)
{
    if (!enabled) {
        ui->DirectoryStructureTreeView->setModel(nullptr);
        this->directoryTreeModel.reset();
        ui->EntryListView->setModel(nullptr);
        this->entryListModel.reset();
    }
    ui->AddEntryPushButton->setEnabled(enabled);
    ui->DeleteEntryPushButton->setEnabled(enabled);
    ui->ActionDatabaseSettings->setEnabled(enabled);
    ui->ActionDatabaseClose->setEnabled(enabled);
}

void MainWindow::closeDatabase()
{
    this->databaseHandler.reset();
    this->setDatabaseControlsEnabled(false);
}
