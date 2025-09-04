#pragma once

struct ResultatsDevis_CinqPostes
{
    double volumeParPersonne;
    int nombreCamion;
    int nombreMO;
    double coutMainOeuvre;
    double coutCamion;
    double prixKilometrage;
    double prixEmballage;
    double prixLocMateriel;
};

struct ResultatsDevis
{
    double coutStationnement;
    double fraisRoute;
    double coutAssurance;
    double fraisMonteMeubles;
    double prixSuppAdresse;

    double prixMetreCube;
    double prixTotalHT;
    double arrhes;

    ResultatsDevis_CinqPostes resultatsCinqPostes;
};