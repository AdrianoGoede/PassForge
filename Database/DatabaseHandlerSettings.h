#ifndef DATABASEHANDLERSETTINGS_H
#define DATABASEHANDLERSETTINGS_H

#include <QByteArray>

class DatabaseHandlerSettings
{
public:
    DatabaseHandlerSettings();
    DatabaseHandlerSettings(const QByteArray& generalSectionJson, const QByteArray& securitySectionJson);
    ~DatabaseHandlerSettings();
    QByteArray getGeneralJson() const;
    QByteArray getSecurityJson() const;

    const QByteArray& getLanguage() const;
    void setLanguage(const QByteArray& newLanguage);

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
    QByteArray language;
    // Security
    int timeToClearClipboard, timeInactiveToBlock;
    bool lockWhenMachineBlocked, lockWhenMinimizing;
};

#endif // DATABASEHANDLERSETTINGS_H
