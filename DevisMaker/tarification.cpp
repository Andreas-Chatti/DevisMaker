#include "Tarification.h"
#include <cmath>

double Tarification::calculerVolumeParPersonne(double volume, Prestation prestation) const
{
    double volumeParPersonne{ 8.0 };

    // Volume par personne selon le type de prestation
    switch (prestation)
    {
    case Prestation::luxe: return volumeParPersonne = 6.0;  // 6m³ par personne en LUXE

    case Prestation::standard: return volumeParPersonne = 8.0;  // 8m³ par personne en STANDARD

    case Prestation::eco: [[fallthrough]]

    case Prestation::ecoPlus: return volumeParPersonne = 10.0; // 10m³ par personne en ECO/ECO+
    }
}


int Tarification::calculerNombreCamion(double volume, Prestation prestation, Nature nature, double distance, bool accesComplexe, bool montageImportant) const 
{
    int nombreJours{ 1 }; // Par défaut un jour minimum


    // 2. Calcul selon le type de déménagement (urbain ou route)
    // Calcul pour déménagement urbain
    if (nature == Nature::urbain)
    {
        if (volume > 60.0)
            nombreJours++; // +1 jour pour grand volume

        // Complexité supplémentaire pour démontage/remontage en urbain
        if (montageImportant && (prestation == Prestation::luxe || prestation == Prestation::standard || prestation == Prestation::ecoPlus))
            nombreJours++;

        // Accès difficile peut ajouter un jour
        if (accesComplexe && volume > 40.0)
            nombreJours++;
    }


    // Calcul pour déménagement national (route)
    else if (nature == Nature::route)
    {
        bool petitVolume{ volume <= 20.0 };

        if (distance >= 150 && distance <= 400)
            nombreJours = petitVolume ? 2 : 3;

        else if (distance > 400 && distance <= 600)
            nombreJours = 3;

        else if (distance > 600 && distance <= 760)
            nombreJours = 4;

        else if (distance > 760 && distance <= 900)
            nombreJours = 5;

        else if (distance > 900)
            nombreJours = 5 + static_cast<int>((distance - 900) / 300); // +1 jour par 300km supplémentaires
    }


    else if (nature == Nature::groupage)
        nombreJours = 5;


    // Jour de préparation pour prestation LUXE
    if (prestation == Prestation::luxe)
        nombreJours += 1; // Jour supplémentaire pour emballage


    return nombreJours;
}