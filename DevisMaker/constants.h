#pragma once
#include <qstring.h>

class SettingsConstants
{
public:

    // Constantes métier principales
    static constexpr double INSURANCE_RATE_PER_M3{ 500.0 };
    static constexpr double MAX_INSURANCE_VALUE{ 45000.0 };
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

    struct M3PriceRoute
    {
        static constexpr double M3_PRICE_DISTANCE_1{ 65.0 }; // Prix M3 entre 150 et 400 kms
        static constexpr double M3_PRICE_DISTANCE_2{ 75.0 }; // Prix M3 supérieur à 400 kms et inférieur ou égale à 600 kms
        static constexpr double M3_PRICE_DISTANCE_3{ 80.0 }; // Prix M3 supérieur à 600 kms et inférieur ou égale à 760 kms
        static constexpr double M3_PRICE_DISTANCE_4{ 100.0 }; // Prix M3 supérieur à 760 kms et inférieur ou égale à 900 kms
        static constexpr double M3_PRICE_DISTANCE_5{ 120.0 }; // Prix M3 supérieur à 900 kms
    };

    struct M3PriceUrban
    {
        static constexpr double M3_PRICE_ECO_LOW{ 30.0 };
        static constexpr double M3_PRICE_ECOPLUS_LOW{ 35.0 };
        static constexpr double M3_PRICE_STANDARD_LOW{ 40.0 };
        static constexpr double M3_PRICE_LUXE_LOW{ 50.0 };
    };

    // Capacités camions
    static constexpr double TRUCK_CAPACITY_ECO = 35.0;
    static constexpr double TRUCK_CAPACITY_LUXE = 25.0;

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
    };

    // Facteurs calculés (évite la duplication)
    static constexpr double TVA_MULTIPLIER{ 1.0 + TVA_RATE };
    static constexpr double EMISSION_CO2_PER_KM{ 0.42 };  // kg CO2/km

private:

    SettingsConstants() = delete;
};