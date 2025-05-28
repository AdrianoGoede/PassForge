#ifndef APIKEYENTRYEDITOR_H
#define APIKEYENTRYEDITOR_H

#include "../Database/ApiKeyEntry.h"
#include <QDialog>

namespace Ui {
class ApiKeyEntryEditor;
}

class ApiKeyEntryEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ApiKeyEntryEditor(QWidget* parent = nullptr, ApiKeyEntry* dbEntry = nullptr);
    ~ApiKeyEntryEditor();

private slots:
    void setKeyVisible(bool visible);
    void accept() override;

private:
    Ui::ApiKeyEntryEditor* ui;
    ApiKeyEntry* dbEntry;
};

#endif // APIKEYENTRYEDITOR_H
