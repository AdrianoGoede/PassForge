#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QDialog>
#include <QString>

namespace Ui {
class PasswordGenerator;
}

class PasswordGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordGenerator(QWidget *parent = nullptr);
    ~PasswordGenerator();

private slots:
    void generatePassword();
    void copyPasswordToClipboard();
    void addWordlist();
    void removeWordlist();

private:
    Ui::PasswordGenerator *ui;

    void setHashingAlgorithms();
    void setDefaultWordlists();
    std::string getSelectedCharaters() const;
    QString generateRandomPassword() const;
    QString generatePassphrase() const;
    QString generateHash() const;
};

#endif // PASSWORDGENERATOR_H
