#pragma once
#include <QString>

namespace SettingsConstants
{
    // Constantes métier principales
    constexpr double INSURANCE_RATE_PER_M3{ 500.0 };
    constexpr double MAX_INSURANCE_VALUE{ 45000.0 };
    constexpr double CONTRACTUELLE_INSURANCE_RATE{ 0.2 };
    constexpr double DOMMAGE_INSURANCE_RATE{ 0.5 };
    constexpr double TVA_RATE{ 0.20 };
    constexpr double ARRHES_RATE{ 0.30 };
    constexpr double MAX_M3_PER_TRUCK{ 60.0 };
    constexpr int MIN_WORKING_DAY{ 1 };

    namespace Distances
    {
        constexpr double URBAN_DISTANCE_LIMIT{ 150.0 };
        constexpr double ROUTE_DISTANCE_LIMIT_1{ 400.0 };
        constexpr double ROUTE_DISTANCE_LIMIT_2{ 600.0 };
        constexpr double ROUTE_DISTANCE_LIMIT_3{ 760.0 };
        constexpr double ROUTE_DISTANCE_LIMIT_4{ 900.0 };
    }

    namespace Worker
    {
        constexpr int MIN_WORKERS{ 2 };
        constexpr double M3_PER_WORKER_ECO{ 10.0 };
        constexpr double M3_PER_WORKER_ECOPLUS{ 9.0 };
        constexpr double M3_PER_WORKER_STANDARD{ 8.0 };
        constexpr double M3_PER_WORKER_LUXE{ 6.0 };
    }

    // Validité devis
    constexpr int DEVIS_VALIDITY_DAYS{ 60 };

    // Limites validation
    constexpr double MIN_VOLUME{ 0.1 };
    constexpr double MAX_VOLUME{ 1000.0 };
    constexpr double MIN_DISTANCE{ 1.0 };
    constexpr double MAX_DISTANCE{ 2000.0 };

    // Facteurs calculés (évite la duplication)
    constexpr double TVA_MULTIPLIER{ 1.0 + TVA_RATE };
    constexpr double EMISSION_CO2_PER_KM{ 0.42 };  // kg CO2/km

    namespace FileSettings
    {
        inline const QString TEMPLATE_FILE_PATH{ "ressources/templates" };
        inline const QString DATA_FILE_PATH{ "ressources/data" };
    }
}
