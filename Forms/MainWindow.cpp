#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "PasswordGenerator.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_clicked()
{
    PasswordGenerator gen(this);
    gen.exec();
}
