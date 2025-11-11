#include "FileManager.h"
#include <QCoreApplication>

bool FileManager::createRessourcesDirectory()
{
    return createDirectory(getRessourcesPath());
}

bool FileManager::createDataDirectory()
{
    if (!createRessourcesDirectory())
        return false;

    return createDirectory(getDataPath());
}

bool FileManager::createTemplatesDirectory()
{
    if (!createRessourcesDirectory())
        return false;

    return createDirectory(getTemplatesPath());
}

bool FileManager::createPromptsDirectory()
{
    if (!createDataDirectory())
        return false;

    return createDirectory(getPromptsPath());
}

bool FileManager::createModelsDirectory()
{
    if (!createDataDirectory())
        return false;

    return createDirectory(getModelsPath());
}

bool FileManager::createAllDirectories()
{
    bool success{ true };

    success &= createRessourcesDirectory();
    success &= createDataDirectory();
    success &= createTemplatesDirectory();
    success &= createPromptsDirectory();
    success &= createModelsDirectory();

    return success;
}

QString FileManager::getRessourcesPath()
{
    return QCoreApplication::applicationDirPath() + "/" + RESSOURCES_DIR;
}

QString FileManager::getDataPath()
{
    return getRessourcesPath() + "/" + DATA_DIR;
}

QString FileManager::getTemplatesPath()
{
    return getRessourcesPath() + "/" + TEMPLATES_DIR;
}

QString FileManager::getPromptsPath()
{
    return getDataPath() + "/" + PROMPTS_DIR;
}

QString FileManager::getModelsPath()
{
    return getDataPath() + "/" + MODELS_DIR;
}

bool FileManager::fileExists(const QString& filePath)
{
    QFileInfo fileInfo{ filePath };
    return fileInfo.exists() && fileInfo.isFile();
}

bool FileManager::directoryExists(const QString& directoryPath)
{
    QDir dir{ directoryPath };
    return dir.exists();
}

bool FileManager::createDirectory(const QString& path)
{
    QDir dir{ path };

    if (dir.exists())
        return true;

    return dir.mkpath(".");
}
