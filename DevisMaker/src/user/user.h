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

    explicit User(QObject* parent = nullptr);

    ~User() { saveUserFile(); }

    const QString& getCompanyName() const { return companyName; }
    const QString& getCompanyAddress() const { return companyAddress; }
    const QString& getCompanyPhoneNumber() const { return companyPhoneNumber; }
    const QString& getCompanyMail() const { return companyMail; }
    const QString& getSiretNumber() const { return siretNumber; }
    const QString& getApeNumber() const { return apeNumber; }
    const QString& getTvaNumber() const { return tvaNumber; }
    const QString& getUserName() const { return userName; }
    const QString& getUserPhoneNumber() const { return userPhoneNumber; }
    const QString& getUserMail() const { return userMail; }

    void setCompanyName(QString name) { companyName = std::move(name); }
    void setCompanyAddress(QString address) { companyAddress = std::move(address); }
    void setCompanyPhoneNumber(QString phone) { companyPhoneNumber = std::move(phone); }
    void setCompanyMail(QString mail) { companyMail = std::move(mail); }
    void setSiretNumber(QString siret) { siretNumber = std::move(siret); }
    void setApeNumber(QString ape) { apeNumber = std::move(ape); }
    void setTvaNumber(QString tva) { tvaNumber = std::move(tva); }
    void setUserName(QString name) { userName = std::move(name); }
    void setUserPhoneNumber(QString phone) { userPhoneNumber = std::move(phone); }
    void setUserMail(QString mail) { userMail = std::move(mail); }

    bool saveUserFile();

private:

    QString companyName{};
    QString companyAddress{};
    QString companyPhoneNumber{};
    QString companyMail{};
    QString siretNumber{};
    QString apeNumber{};
    QString tvaNumber{};
    QString userName{};
    QString userPhoneNumber{};
    QString userMail{};

    static inline const QString USER_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/userConfig.json" };

    void createUserFile();
    bool loadUserFile();
};