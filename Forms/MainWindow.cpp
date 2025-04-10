#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "DatabaseCreator.h"
#include "PasswordGenerator.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->ActionDatabaseNew, &QAction::triggered, this, &MainWindow::CreateNewDatabase);
    connect(ui->ActionToolsPasswordGenerator, &QAction::triggered, this, &MainWindow::OpenPasswordGenerator);
    connect(ui->ActionApplicationQuit, &QAction::triggered, this, &MainWindow::QuitApplication);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CreateNewDatabase()
{
    DatabaseCreator creator(this);
    creator.exec();
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
