#pragma once
#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include "utils/constants.h"

class User : public QObject
{
    Q_OBJECT

public:

    User(QObject* parent = nullptr);

    ~User() = default;

    QString getCompanyName() const { return companyName; }
    QString getCompanyAddress() const { return companyAddress; }
    QString getCompanyPhoneNumber() const { return companyPhoneNumber; }
    QString getCompanyMail() const { return companyMail; }
    QString getSiretNumber() const { return siretNumber; }
    QString getApeNumber() const { return apeNumber; }
    QString getTvaNumber() const { return tvaNumber; }
    QString getUserName() const { return userName; }
    QString getUserPhoneNumber() const { return userPhoneNumber; }
    QString getUserMail() const { return userMail; }

    void setCompanyName(const QString& name) { companyName = name; }
    void setCompanyAddress(const QString& address) { companyAddress = address; }
    void setCompanyPhoneNumber(const QString& phone) { companyPhoneNumber = phone; }
    void setCompanyMail(const QString& mail) { companyMail = mail; }
    void setSiretNumber(const QString& siret) { siretNumber = siret; }
    void setApeNumber(const QString& ape) { apeNumber = ape; }
    void setTvaNumber(const QString& tva) { tvaNumber = tva; }
    void setUserName(const QString& name) { userName = name; }
    void setUserPhoneNumber(const QString& phone) { userPhoneNumber = phone; }
    void setUserMail(const QString& mail) { userMail = mail; }

    bool saveUserFile();

private:

    QString companyName;
    QString companyAddress;
    QString companyPhoneNumber;
    QString companyMail;
    QString siretNumber;
    QString apeNumber;
    QString tvaNumber;
    QString userName;
    QString userPhoneNumber;
    QString userMail;

    static inline const QString USER_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/userConfig.json" };

    void createUserFile();
    bool loadUserFile();
};