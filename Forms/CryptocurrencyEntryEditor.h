#ifndef CRYPTOCURRENCYENTRYEDITOR_H
#define CRYPTOCURRENCYENTRYEDITOR_H

#include "../Database/CryptocurrencyEntry.h"
#include <QDialog>

namespace Ui {
class CryptocurrencyEntryEditor;
}

class CryptocurrencyEntryEditor : public QDialog
{
    Q_OBJECT

public:
    explicit CryptocurrencyEntryEditor(QWidget* parent = nullptr, CryptocurrencyEntry* dbEntry = nullptr);
    ~CryptocurrencyEntryEditor();

private slots:
    void setSeedVisible(bool visible);
    void setMasterKeyVisible(bool visible);
    void accept() override;

private:
    Ui::CryptocurrencyEntryEditor *ui;
    CryptocurrencyEntry* dbEntry;
};

#endif // CRYPTOCURRENCYENTRYEDITOR_H
