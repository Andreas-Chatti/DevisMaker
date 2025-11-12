#include "AIModel.h"

AIModel AIModel::makeDefaultModel(QObject* parent)
{
	return AIModel{ AIModel::DEFAULT_MODEL_NAME, AIModel::MAX_OUTPUT_TOKENS, AIModel::MIN_TEMPERATURE, AIModel::DEFAULT_URL, parent };
}

void AIModel::initializationCheck()
{
	Q_ASSERT(!m_modelName.isEmpty());
	if (m_modelName.isEmpty())
		throw std::invalid_argument("AI model was initialized with an empty name.");

	Q_ASSERT(!m_url.isEmpty());
	if (m_url.isEmpty())
		throw std::invalid_argument("AI model was initialized with an empty URL.");

	if (m_maxOutputTokens < MIN_OUTPUT_TOKENS || m_maxOutputTokens > MAX_OUTPUT_TOKENS)
		m_maxOutputTokens = MAX_OUTPUT_TOKENS;

	if (m_temperature < MIN_TEMPERATURE || m_temperature > MAX_TEMPERATURE)
		m_temperature = MIN_TEMPERATURE;
}