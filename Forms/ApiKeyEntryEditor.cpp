#include "ApiKeyEntryEditor.h"
#include "ui_ApiKeyEntryEditor.h"

ApiKeyEntryEditor::ApiKeyEntryEditor(QWidget* parent, ApiKeyEntry* dbEntry) : QDialog(parent), dbEntry(dbEntry), ui(new Ui::ApiKeyEntryEditor)
{
    if (!this->dbEntry) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);

    ui->NameLineEdit->setText(QString::fromUtf8(this->dbEntry->getName()));
    ui->PathLineEdit->setText(QString::fromUtf8(this->dbEntry->getPath()));
    ui->UrlLineEdit->setText(QString::fromUtf8(this->dbEntry->getUrl()));
    ui->KeyLineEdit->setText(QString::fromUtf8(this->dbEntry->getKey()));
    ui->NotesPlainTextEdit->setPlainText(QString::fromUtf8(this->dbEntry->getNotes()));

    connect(ui->KeyShowPushButton, &QAbstractButton::clicked, this, &ApiKeyEntryEditor::setKeyVisible);
    connect(ui->SavePushButton, &QAbstractButton::clicked, this, &ApiKeyEntryEditor::accept);
    connect(ui->CancelPushButton, &QAbstractButton::clicked, this, &ApiKeyEntryEditor::reject);
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
