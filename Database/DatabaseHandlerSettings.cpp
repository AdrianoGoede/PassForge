#include "DatabaseHandlerSettings.h"
#include "../Crypto/Crypto.h"
#include <QJsonDocument>
#include <QJsonObject>

DatabaseHandlerSettings::DatabaseHandlerSettings() {}

DatabaseHandlerSettings::DatabaseHandlerSettings(const QByteArray& generalSectionJson, const QByteArray& securitySectionJson)
{
    QJsonObject generalSection = QJsonDocument::fromJson(generalSectionJson).object();
    this->language = SecureQByteArray(generalSection["language"].toString("").toUtf8());

    QJsonObject securitySection = QJsonDocument::fromJson(securitySectionJson).object();
    this->timeToClearClipboard = securitySection["timeToClearClipboard"].toInt(0);
    this->timeInactiveToBlock = securitySection["timeInactiveToBlock"].toInt(0);
    this->lockWhenMachineBlocked = securitySection["lockWhenMachineBlocked"].toBool(false);
    this->lockWhenMinimizing = securitySection["lockWhenMinimizing"].toBool(false);
}

DatabaseHandlerSettings::~DatabaseHandlerSettings()
{
    Crypto::wipeMemory(&this->timeToClearClipboard, sizeof(this->timeToClearClipboard));
    Crypto::wipeMemory(&this->timeInactiveToBlock, sizeof(this->timeInactiveToBlock));
    Crypto::wipeMemory(&this->lockWhenMachineBlocked, sizeof(this->lockWhenMachineBlocked));
    Crypto::wipeMemory(&this->lockWhenMinimizing, sizeof(this->lockWhenMinimizing));
}

SecureQByteArray DatabaseHandlerSettings::getGeneralJson() const
{
    QJsonObject obj;
    obj["language"] = this->language.data();
    return SecureQByteArray(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact));
}

SecureQByteArray DatabaseHandlerSettings::getSecurityJson() const
{
    QJsonObject obj;
    obj["timeToClearClipboard"] = this->timeToClearClipboard;
    obj["timeInactiveToBlock"] = this->timeInactiveToBlock;
    obj["lockWhenMachineBlocked"] = this->lockWhenMachineBlocked;
    obj["lockWhenMinimizing"] = this->lockWhenMinimizing;
    return SecureQByteArray(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact));
}

const SecureQByteArray& DatabaseHandlerSettings::getLanguage() const { return this->language; }

void DatabaseHandlerSettings::setLanguage(QByteArray&& newLanguage) { this->language = SecureQByteArray(std::move(newLanguage)); }

int DatabaseHandlerSettings::getTimeToClearClipboard() const { return this->timeToClearClipboard; }

void DatabaseHandlerSettings::setTimeToClearClipboard(int newTimeToClearClipboard) { this->timeToClearClipboard = newTimeToClearClipboard; }

int DatabaseHandlerSettings::getTimeInactiveToBlock() const { return this->timeInactiveToBlock; }

void DatabaseHandlerSettings::setTimeInactiveToBlock(int newTimeInactiveToBlock) { this->timeInactiveToBlock = newTimeInactiveToBlock; }

bool DatabaseHandlerSettings::getLockWhenMachineBlocked() const { return this->lockWhenMachineBlocked; }

void DatabaseHandlerSettings::setLockWhenMachineBlocked(bool newLockWhenMachineBlocked){ this->lockWhenMachineBlocked = newLockWhenMachineBlocked; }

bool DatabaseHandlerSettings::getLockWhenMinimizing() const { return this->lockWhenMinimizing; }

void DatabaseHandlerSettings::setLockWhenMinimizing(bool newLockWhenMinimizing) { this->lockWhenMinimizing = newLockWhenMinimizing; }
