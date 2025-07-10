#pragma once
#include <qstring.h>


class SettingsConstants
{
public:

    enum class CalculationType
    {
        PricePerM3,
        FivePostes,
    };

    // Constantes métier principales
    static constexpr double INSURANCE_RATE_PER_M3{ 500.0 };
    static constexpr double MAX_INSURANCE_VALUE{ 45000.0 };
    static constexpr double CONTRACTUELLE_INSURANCE_RATE{ 0.2 };
    static constexpr double DOMMAGE_INSURANCE_RATE{ 0.5 };
    static constexpr double TVA_RATE{ 0.20 };
    static constexpr double ARRHES_RATE{ 0.30 };
    static constexpr double MAX_M3_PER_TRUCK{ 60.0 };
    static constexpr int MIN_WORKING_DAY{ 1 };

    struct Distances
    {
        static constexpr double URBAN_DISTANCE_LIMIT{ 150.0 };
        static constexpr double ROUTE_DISTANCE_LIMIT_1{ 400.0 };
        static constexpr double ROUTE_DISTANCE_LIMIT_2{ 600.0 };
        static constexpr double ROUTE_DISTANCE_LIMIT_3{ 760.0 };
        static constexpr double ROUTE_DISTANCE_LIMIT_4{ 900.0 };
    };

    struct Worker
    {
        static constexpr int MIN_WORKERS{ 2 };
        static constexpr double M3_PER_WORKER_ECO{ 10.0 };
        static constexpr double M3_PER_WORKER_ECOPLUS{ 9.0 };
        static constexpr double M3_PER_WORKER_STANDARD{ 8.0 };
        static constexpr double M3_PER_WORKER_LUXE{ 6.0 };
    };

    // Validité devis
    static constexpr int DEVIS_VALIDITY_DAYS{ 60 };

    // Limites validation
    static constexpr double MIN_VOLUME{ 0.1 };
    static constexpr double MAX_VOLUME{ 1000.0 };
    static constexpr double MIN_DISTANCE{ 1.0 };
    static constexpr double MAX_DISTANCE{ 2000.0 };

    struct CompanyInfos
    {
        static inline const QString COMPANY_NAME{ "CHATTI DEMENAGEMENT" };
        static inline const QString COMPANY_ADDRESS{ "2, Rue Jean Cocteau\n91460 MARCOUSSIS" };
        static inline const QString COMPANY_PHONE_1{ "01 69 75 18 22" };
        static inline const QString COMPANY_PHONE_2{ "06 72 85 35 38" };
        static inline const QString COMPANY_EMAIL{ "chattidemenagement@gmail.com" };
        static inline const QString COMPANY_SIRET{ "39032989400054" };
        static inline const QString COMPANY_TVA{ "FR68390329894" };
        static inline const QString COMPANY_APE{ "4942Z" };
    };

    // Facteurs calculés (évite la duplication)
    static constexpr double TVA_MULTIPLIER{ 1.0 + TVA_RATE };
    static constexpr double EMISSION_CO2_PER_KM{ 0.42 };  // kg CO2/km

private:

    SettingsConstants() = delete;
    ~SettingsConstants() = delete;
};