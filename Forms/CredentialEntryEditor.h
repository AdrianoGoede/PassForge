#ifndef CREDENTIALENTRYEDITOR_H
#define CREDENTIALENTRYEDITOR_H

#include "../Database/CredentialEntry.h"
#include <QDialog>

namespace Ui {
class CredentialEntryEditor;
}

class CredentialEntryEditor : public QDialog
{
    Q_OBJECT

public:
    explicit CredentialEntryEditor(QWidget *parent = nullptr, CredentialEntry* dbEntry = nullptr);
    ~CredentialEntryEditor();

private slots:
    void setPasswordVisible(bool visible);
    void accept() override;

private:
    Ui::CredentialEntryEditor *ui;
    CredentialEntry* dbEntry;
};

#endif // CREDENTIALENTRYEDITOR_H
