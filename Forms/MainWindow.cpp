#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DatabaseCreator.h"
#include "DatabaseSettings.h"
#include "PasswordGenerator.h"
#include "../Configs/Configs.h"
#include "../Crypto/Crypto.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->CentralWidget->setLayout(ui->CentralGridLayout);
    ui->HeaderInfoTabCredential->setLayout(ui->CredentialHeaderInfoLayout);
    ui->HeaderInfoTabCryptocurrency->setLayout(ui->CryptocurrencyHeaderInfoLayout);
    ui->HeaderInfoTabApiKey->setLayout(ui->ApiKeyHeaderInfoLayout);
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
        QString dbPath = QFileDialog::getOpenFileName(this, "", QString(), DATABASE_FILE_FILTER);
        if (dbPath.isEmpty()) return;

        bool ok;
        QByteArray password = QInputDialog::getText(this, "Enter database password", "Password", QLineEdit::EchoMode::Password, "", &ok).toUtf8();
        if (!ok) return;
        if (password.isEmpty()) throw std::runtime_error("Database password cannot be empty");

        this->databaseHandler.reset(new DatabaseHandler(dbPath, password));
        Crypto::wipeMemory(password.data(), password.length());
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

void MainWindow::QuitApplication() { QApplication::closeAllWindows(); }

void MainWindow::loadDatabase()
{

}
