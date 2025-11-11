#include "user/user.h"

User::User(QObject* parent)
	: QObject(parent)
{
	if (!loadUserFile())
		createUserFile();
}


bool User::loadUserFile()
{
    QFile userFile{ USER_FILE_PATH };

    if (!userFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray fileRawData{ userFile.readAll() };
    userFile.close();

    QJsonParseError error;
    QJsonDocument jsonDocument{ QJsonDocument::fromJson(fileRawData, &error) };

    if (error.error != QJsonParseError::NoError)
        return false;

    QJsonObject jsonBody{ jsonDocument.object() };

    companyName = jsonBody["companyName"].toString();
    companyAddress = jsonBody["companyAddress"].toString();
    companyPhoneNumber = jsonBody["companyPhoneNumber"].toString();
    companyMail = jsonBody["companyMail"].toString();
    siretNumber = jsonBody["siretNumber"].toString();
    apeNumber = jsonBody["apeNumber"].toString();
    tvaNumber = jsonBody["tvaNumber"].toString();
    userName = jsonBody["userName"].toString();
    userPhoneNumber = jsonBody["userPhoneNumber"].toString();
    userMail = jsonBody["userMail"].toString();

    return true;
}


bool User::saveUserFile()
{
    QFile userFile{ USER_FILE_PATH };

    if (!userFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonObject jsonBody;

    jsonBody["companyName"] = companyName;
    jsonBody["companyAddress"] = companyAddress;
    jsonBody["companyPhoneNumber"] = companyPhoneNumber;
    jsonBody["companyMail"] = companyMail;
    jsonBody["siretNumber"] = siretNumber;
    jsonBody["apeNumber"] = apeNumber;
    jsonBody["tvaNumber"] = tvaNumber;
    jsonBody["userName"] = userName;
    jsonBody["userPhoneNumber"] = userPhoneNumber;
    jsonBody["userMail"] = userMail;

    QJsonDocument jsonDocument{ jsonBody };
    userFile.write(jsonDocument.toJson());
    userFile.close();

    return true;
}


void User::createUserFile()
{
    QFileInfo fileInfo(USER_FILE_PATH);
    QDir dataDir{ FileManager::getDataPath() };
    if (!dataDir.exists())
        FileManager::createDataDirectory();

    QFile userFile{ USER_FILE_PATH };

    if (!userFile.open(QIODevice::WriteOnly | QIODevice::Text)) 
    {
        qWarning() << "Impossible de créer le fichier utilisateur:" << USER_FILE_PATH;
        return;
    }

    QJsonObject jsonBody;

    jsonBody["companyName"] = "";
    jsonBody["companyAddress"] = "";
    jsonBody["companyPhoneNumber"] = "";
    jsonBody["companyMail"] = "";
    jsonBody["siretNumber"] = "";
    jsonBody["apeNumber"] = "";
    jsonBody["tvaNumber"] = "";
    jsonBody["userName"] = "";
    jsonBody["userPhoneNumber"] = "";
    jsonBody["userMail"] = "";

    QJsonDocument jsonDocument{ jsonBody };
    userFile.write(jsonDocument.toJson(QJsonDocument::Indented));
    userFile.close();
}