#pragma once
#include <QString>
#include <vector>
#include <memory>
#include "inventory/movingObject.h"

class Area
{
public:

	enum class AreaType
	{
		none,                    // Aucune

		// Pièces principales
		living_room,             // Salon
		dining_room,             // Salle à manger
		kitchen,                 // Cuisine
		bedroom,                 // Chambre
		master_bedroom,          // Chambre principale
		guest_room,              // Chambre d'amis
		children_room,           // Chambre d'enfant

		// Sanitaires
		bathroom,                // Salle de bain
		toilet,                  // Toilettes / WC
		shower_room,             // Salle d'eau
		powder_room,             // Cabinet de toilette

		// Espaces de travail/loisirs
		office,                  // Bureau
		study,                   // Cabinet de travail
		library,                 // Bibliothèque
		playroom,                // Salle de jeux
		game_room,               // Salle de jeux (adultes)
		gym,                     // Salle de sport
		music_room,              // Salle de musique
		home_theater,            // Home cinéma

		// Espaces de service
		laundry_room,            // Buanderie
		utility_room,            // Arrière-cuisine
		storage_room,            // Débarras
		pantry,                  // Garde-manger
		closet,                  // Placard
		walk_in_closet,          // Dressing (walk-in)
		dressing_room,           // Dressing
		workshop,                // Atelier
		boiler_room,             // Chaufferie

		// Circulation
		hallway,                 // Couloir
		corridor,                // Corridor
		entrance,                // Entrée
		foyer,                   // Hall d'entrée
		staircase,               // Cage d'escalier
		landing,                 // Palier
		vestibule,               // Vestibule

		// Niveaux particuliers
		attic,                   // Grenier
		loft,                    // Loft / Mezzanine
		basement,                // Sous-sol
		cellar,                  // Cave
		wine_cellar,             // Cave à vin
		crawl_space,             // Vide sanitaire

		// Garage/parking
		garage,                  // Garage
		carport,                 // Abri voiture
		parking_space,           // Place de parking

		// Extérieur - espaces de vie
		garden,                  // Jardin
		terrace,                 // Terrasse
		patio,                   // Patio
		balcony,                 // Balcon
		veranda,                 // Véranda
		porch,                   // Porche
		deck,                    // Terrasse en bois
		loggia,                  // Loggia

		// Extérieur - cours
		backyard,                // Arrière-cour
		front_yard,              // Cour avant
		courtyard,               // Cour intérieure
		side_yard,               // Cour latérale

		// Extérieur - installations
		pool_area,               // Espace piscine
		shed,                    // Cabanon / Remise
		greenhouse,              // Serre
		pergola,                 // Pergola
		gazebo,                  // Kiosque / Gloriette
		driveway,                // Allée
		barn,                    // Grange
		stable,                  // Écurie

		// Annexes / Dépendances
		warehouse,               // Entrepôt / Hangar
		storage_unit,            // Box de stockage
		outbuilding,             // Dépendance / Annexe
		lean_to,                 // Appentis
		tool_shed,               // Cabane à outils
		garden_shed,             // Abri de jardin
		depot,                   // Dépôt
		hangar,                  // Hangar (avion)
		storeroom,               // Magasin / Réserve
		stockroom,               // Réserve
		vault,                   // Coffre-fort / Chambre forte
		safe_room,               // Pièce forte

		max_room_types
	};

	explicit Area(QString name, AreaType type);

	// Gestion des objets
	void addObject(const MovingObject& object);
	void removeObject(const QString& objectName);
	const std::vector<MovingObject>& getObjects() const { return m_objects; }

	// Getters
	const QString& getName() const { return m_name; }
	AreaType getType() const { return m_type; }
	double getTotalVolume() const;
	int getObjectCount() const { return m_objects.size(); }

	// Setters
	void setName(QString name) { m_name = std::move(name); }
	void setType(AreaType type) { m_type = type; }

	// Utilitaire
	static QString AreaTypeToString(AreaType type);

private:
	QString m_name;                      // "Chambre d'amis 1", "Salon principal"
	AreaType m_type;                     // Le type générique
	std::vector<MovingObject> m_objects{}; // Les objets dans cette pièce
};