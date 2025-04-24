#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DatabaseCreator.h"
#include "DatabaseSettings.h"
#include "PasswordGenerator.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->CentralWidget->setLayout(ui->CentralGridLayout);
    ui->HeaderInfoTabCredential->setLayout(ui->CredentialHeaderInfoLayout);
    ui->HeaderInfoTabCryptocurrency->setLayout(ui->CryptocurrencyHeaderInfoLayout);
    connect(ui->ActionDatabaseNew, &QAction::triggered, this, &MainWindow::CreateNewDatabase);
    connect(ui->ActionDatabaseSettings, &QAction::triggered, this, &MainWindow::OpenDatabaseSettings);
    connect(ui->ActionToolsPasswordGenerator, &QAction::triggered, this, &MainWindow::OpenPasswordGenerator);
    connect(ui->ActionApplicationQuit, &QAction::triggered, this, &MainWindow::QuitApplication);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CreateNewDatabase()
{
    DatabaseCreator creator(this);
    if (creator.exec() == QDialog::Accepted) {
        try {
            this->databaseHandler.reset(creator.getDatabaseHandler());
        }
        catch (const std::exception& exception) { QMessageBox::critical(this, "Error", exception.what()); }
    }
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

void MainWindow::QuitApplication()
{
    QApplication::closeAllWindows();
}
