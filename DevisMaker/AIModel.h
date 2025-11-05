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

private:

	QString m_modelName;
	int m_maxOutputTokens;
	double m_temperature;
	QUrl m_url;
};