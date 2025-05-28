#include "CryptocurrencyEntryEditor.h"
#include "ui_CryptocurrencyEntryEditor.h"

CryptocurrencyEntryEditor::CryptocurrencyEntryEditor(QWidget* parent, CryptocurrencyEntry* dbEntry) : QDialog(parent), dbEntry(dbEntry), ui(new Ui::CryptocurrencyEntryEditor)
{
    if (!this->dbEntry) throw std::runtime_error("dbEntry must be passed");
    ui->setupUi(this);

    ui->NameLineEdit->setText(QString::fromUtf8(this->dbEntry->getName()));
    ui->PathLineEdit->setText(QString::fromUtf8(this->dbEntry->getPath()));
    ui->CryptocurrencyNameLineEdit->setText(QString::fromUtf8(this->dbEntry->getCryptocurrencyName()));
    ui->SeedLineEdit->setText(QString::fromUtf8(this->dbEntry->getSeed()));
    ui->MasterKeyLineEdit->setText(QString::fromUtf8(this->dbEntry->getMasterPrivateKey()));
    ui->NotesPlainTextEdit->setPlainText(QString::fromUtf8(this->dbEntry->getNotes()));

    connect(ui->SeedShowPushButton, &QAbstractButton::clicked, this, &CryptocurrencyEntryEditor::setSeedVisible);
    connect(ui->MasterKeyShowPushButton, &QAbstractButton::clicked, this, &CryptocurrencyEntryEditor::setMasterKeyVisible);
    connect(ui->SavePushButton, &QAbstractButton::clicked, this, &CryptocurrencyEntryEditor::accept);
    connect(ui->CancelPushButton, &QAbstractButton::clicked, this, &CryptocurrencyEntryEditor::reject);
}

CryptocurrencyEntryEditor::~CryptocurrencyEntryEditor() { delete ui; }

void CryptocurrencyEntryEditor::setSeedVisible(bool visible) { ui->SeedLineEdit->setEchoMode(visible ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password); }

void CryptocurrencyEntryEditor::setMasterKeyVisible(bool visible) { ui->MasterKeyLineEdit->setEchoMode(visible ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password); }

void CryptocurrencyEntryEditor::accept()
{
    this->dbEntry->setName(ui->NameLineEdit->text().toUtf8());
    this->dbEntry->setPath(ui->PathLineEdit->text().toUtf8());
    this->dbEntry->setCryptocurrencyName(ui->CryptocurrencyNameLineEdit->text().toUtf8());
    this->dbEntry->setSeed(ui->SeedLineEdit->text().toUtf8());
    this->dbEntry->setMasterPrivateKey(ui->MasterKeyLineEdit->text().toUtf8());
    this->dbEntry->setNotes(ui->NotesPlainTextEdit->toPlainText().toUtf8());
    QDialog::accept();
}
