#include "ApiKeyEntryEditor.h"
#include "ui_ApiKeyEntryEditor.h"

#include <QMessageBox>

ApiKeyEntryEditor::ApiKeyEntryEditor(QWidget* parent, ApiKeyEntry* dbEntry) : QDialog(parent), ui(new Ui::ApiKeyEntryEditor)
{
    if (!dbEntry) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);

    this->dbEntry = dbEntry;
    ui->NameLineEdit->setText(QString::fromUtf8(dbEntry->getName()));
    ui->PathLineEdit->setText(QString::fromUtf8(dbEntry->getPath()));
    ui->UrlLineEdit->setText(QString::fromUtf8(dbEntry->getUrl()));
    ui->KeyLineEdit->setText(QString::fromUtf8(dbEntry->getKey()));
    ui->NotesPlainTextEdit->setPlainText(QString::fromUtf8(dbEntry->getNotes()));

    connect(ui->KeyShowPushButton, &QAbstractButton::clicked, this, &ApiKeyEntryEditor::setKeyVisible);
    connect(ui->SavePushButton, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->CancelPushButton, &QAbstractButton::clicked, this, &QDialog::reject);
}

ApiKeyEntryEditor::~ApiKeyEntryEditor() { delete ui; }

void ApiKeyEntryEditor::setKeyVisible(bool visible) { ui->KeyLineEdit->setEchoMode(visible ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password); }

void ApiKeyEntryEditor::accept()
{
    this->dbEntry->setName(ui->NameLineEdit->text().toUtf8());
    this->dbEntry->setPath(ui->PathLineEdit->text().toUtf8());
    this->dbEntry->setUrl(ui->UrlLineEdit->text().toUtf8());
    this->dbEntry->setKey(ui->KeyLineEdit->text().toUtf8());
    this->dbEntry->setNotes(ui->NotesPlainTextEdit->toPlainText().toUtf8());
    QDialog::accept();
}
