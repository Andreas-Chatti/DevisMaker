#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <QJsonObject>
#include <QFile>
#include "utils/constants.h"

class AIModel : public QObject
{
	Q_OBJECT

public:

	AIModel(QString modelName, int maxOutputTokens, double temperature, QUrl url, QObject* parent = nullptr)
		: QObject{ parent }
		, m_modelName{ std::move(modelName) }
		, m_maxOutputTokens{ maxOutputTokens }
		, m_temperature{ temperature }
		, m_url{ std::move(url) }
	{
		try
		{
			initializationCheck();
		}

		catch (std::invalid_argument ex)
		{
			m_modelName = DEFAULT_MODEL_NAME;
			m_url = DEFAULT_URL;
		}
	}

	~AIModel() = default;

	AIModel(const AIModel& aiModel)
		: AIModel(aiModel.m_modelName, aiModel.m_maxOutputTokens, aiModel.m_temperature, aiModel.m_url, aiModel.parent())
	{
	}

	AIModel& operator=(const AIModel& aiModel)
	{
		if (&aiModel == this)
			return *this;

		m_modelName = aiModel.m_modelName;
		m_maxOutputTokens = aiModel.m_maxOutputTokens;
		m_temperature = aiModel.m_temperature;
		m_url = aiModel.m_url;
		setParent(aiModel.parent());

		return *this;
	}

	AIModel(AIModel&& aiModel) noexcept
		: AIModel(aiModel.m_modelName, aiModel.m_maxOutputTokens, aiModel.m_temperature, aiModel.m_url, aiModel.parent())
	{
	}

	AIModel& operator=(AIModel&& aiModel) noexcept
	{
		if (&aiModel == this)
			return *this;

		m_modelName = std::move(aiModel.m_modelName);
		m_maxOutputTokens = aiModel.m_maxOutputTokens;
		m_temperature = aiModel.m_temperature;
		m_url = std::move(aiModel.m_url);
		setParent(aiModel.parent());

		return *this;
	}

	const QString& getModelName() const { return m_modelName; }
	int getMaxOutputTokens() const { return m_maxOutputTokens; }
	double getTemperature() const { return m_temperature; }
	const QUrl& getUrl() const { return m_url; }

	static constexpr int MIN_OUTPUT_TOKENS{ 2000 };
	static constexpr int MAX_OUTPUT_TOKENS{ 16000 };
	static constexpr double MIN_TEMPERATURE{ 0.1 };
	static constexpr double MAX_TEMPERATURE{ 3.0 };
	static inline const QString DEFAULT_MODEL_NAME{ "llama-3.1-8b-instant" };
	static inline const QUrl DEFAULT_URL{ "https://api.groq.com/openai/v1/chat/completions" };

	static AIModel makeDefaultModel(QObject* parent);

private:

	QString m_modelName;
	int m_maxOutputTokens;
	double m_temperature;
	QUrl m_url;

	void initializationCheck();
};