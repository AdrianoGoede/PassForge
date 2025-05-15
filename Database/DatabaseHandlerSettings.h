#ifndef DATABASEHANDLERSETTINGS_H
#define DATABASEHANDLERSETTINGS_H

#include "../Crypto/SecureQByteArray.h"
#include <QByteArray>

class DatabaseHandlerSettings
{
public:
    DatabaseHandlerSettings();
    DatabaseHandlerSettings(const QByteArray& generalSectionJson, const QByteArray& securitySectionJson);
    ~DatabaseHandlerSettings();
    SecureQByteArray getGeneralJson() const;
    SecureQByteArray getSecurityJson() const;

    const SecureQByteArray& getLanguage() const;
    void setLanguage(QByteArray&& newLanguage);

    int getTimeToClearClipboard() const;
    void setTimeToClearClipboard(int newTimeToClearClipboard);

    int getTimeInactiveToBlock() const;
    void setTimeInactiveToBlock(int newTimeInactiveToBlock);

    bool getLockWhenMachineBlocked() const;
    void setLockWhenMachineBlocked(bool newLockWhenMachineBlocked);

    bool getLockWhenMinimizing() const;
    void setLockWhenMinimizing(bool newLockWhenMinimizing);

private:
    // General
    SecureQByteArray language;
    // Security
    int timeToClearClipboard, timeInactiveToBlock;
    bool lockWhenMachineBlocked, lockWhenMinimizing;
};

#endif // DATABASEHANDLERSETTINGS_H
