#pragma once
#include <QString>
#include <QDir>
#include <QCoreApplication>

class FileManager
{
public:

    FileManager() = delete;
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;
    FileManager(FileManager&&) = delete;
    FileManager& operator=(FileManager&&) = delete;

    // Directory creation methods
    static bool createRessourcesDirectory();
    static bool createDataDirectory();
    static bool createTemplatesDirectory();
    static bool createPromptsDirectory();
    static bool createModelsDirectory();
    static bool createLogsDirectory();

    // Convenience method to create all directories at once
    static bool createAllDirectories();

    // Ensure directory structure exists (call at program startup)
    static bool ensureDirectoryStructure();

    // Path getters
    static QString getRessourcesPath();
    static QString getDataPath();
    static QString getTemplatesPath();
    static QString getPromptsPath();
    static QString getModelsPath();
    static QString getLogsPath();

    // File/Directory existence checks
    static bool fileExists(const QString& filePath);
    static bool directoryExists(const QString& directoryPath);

private:

    static constexpr const char* RESSOURCES_DIR{ "ressources" };
    static constexpr const char* DATA_DIR{ "data" };
    static constexpr const char* TEMPLATES_DIR{ "templates" };
    static constexpr const char* PROMPTS_DIR{ "prompts" };
    static constexpr const char* MODELS_DIR{ "models" };
    static constexpr const char* LOGS_DIR{ "logs" };

    static bool createDirectory(const QString& path);
};
