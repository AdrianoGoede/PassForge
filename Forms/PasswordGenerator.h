#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QDialog>
#include <QString>
#include <QByteArray>
#include "../Crypto/SecureQByteArray.h"

namespace Ui {
class PasswordGenerator;
}

class PasswordGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordGenerator(QWidget *parent = nullptr);
    ~PasswordGenerator();
    SecureQByteArray getValue() const;

private slots:
    void generatePassword();
    void validatePassword();
    void copyPasswordToClipboard();
    void addWordlist();
    void removeWordlist();

private:
    Ui::PasswordGenerator *ui;
    QVector<QString> wordlist;
    bool reloadWordlists = true;

    void setHashingAlgorithms();
    void setDefaultWordlists();
    void loadWordlist();
    QString getSelectedCharaters() const;
    SecureQByteArray generateRandomPassword() const;
    SecureQByteArray generatePassphrase();
    SecureQByteArray generateHash() const;
};

#endif // PASSWORDGENERATOR_H
