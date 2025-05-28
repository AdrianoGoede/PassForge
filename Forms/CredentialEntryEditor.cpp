#include "CredentialEntryEditor.h"
#include "ui_CredentialEntryEditor.h"

CredentialEntryEditor::CredentialEntryEditor(QWidget* parent, CredentialEntry* dbEntry) : QDialog(parent), dbEntry(dbEntry), ui(new Ui::CredentialEntryEditor)
{
    if (!this->dbEntry) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);

    ui->NameLineEdit->setText(QString::fromUtf8(this->dbEntry->getName()));
    ui->PathLineEdit->setText(QString::fromUtf8(this->dbEntry->getPath()));
    ui->UsernameLineEdit->setText(QString::fromUtf8(this->dbEntry->getUsername()));
    ui->PasswordLineEdit->setText(QString::fromUtf8(this->dbEntry->getPassword()));
    ui->UrlLineEdit->setText(QString::fromUtf8(this->dbEntry->getUrl()));
    ui->NotesPlainTextEdit->setPlainText(QString::fromUtf8(this->dbEntry->getNotes()));

    connect(ui->PasswordShowPushButton, &QAbstractButton::clicked, this, &CredentialEntryEditor::setPasswordVisible);
    connect(ui->SavePushButton, &QAbstractButton::clicked, this, &CredentialEntryEditor::accept);
    connect(ui->CancelPushButton, &QAbstractButton::clicked, this, &CredentialEntryEditor::reject);
}

CredentialEntryEditor::~CredentialEntryEditor() { delete ui; }

void CredentialEntryEditor::setPasswordVisible(bool visible) { ui->PasswordLineEdit->setEchoMode(visible ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password); }

void CredentialEntryEditor::accept()
{
    this->dbEntry->setName(ui->NameLineEdit->text().toUtf8());
    this->dbEntry->setPath(ui->PathLineEdit->text().toUtf8());
    this->dbEntry->setUsername(ui->UsernameLineEdit->text().toUtf8());
    this->dbEntry->setPassword(ui->PasswordLineEdit->text().toUtf8());
    this->dbEntry->setUrl(ui->UrlLineEdit->text().toUtf8());
    this->dbEntry->setNotes(ui->NotesPlainTextEdit->toPlainText().toUtf8());
    QDialog::accept();
}
