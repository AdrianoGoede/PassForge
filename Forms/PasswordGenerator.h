#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QDialog>

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

private:
    Ui::PasswordGenerator *ui;
    std::string getSelectedCharaters() const;
};

#endif // PASSWORDGENERATOR_H
