# Instructions pour Claude Code

## À propos de ce projet
Je suis en train d'apprendre le C++. Ce projet (DevisMaker) est personnel et éducatif. Mais je code comme s'il s'agissait d'un projet professionnel.
J'utilise les meilleures pratiques professionnelles, les principes SOLID et j'essaie d'utiliser des design patterns dans les bonnes situations sans faire trop
d'over-engineering lorsque ce n'est pas nécessaire.

## Structure du projet
```
DevisMaker/
├── src/              # Fichiers source (.cpp)
├── ressources/       # Ressources du projet
│   ├── data/         # Fichiers de configuration
│   └── templates/    # Templates
├── x64/              # Fichiers de build (ne pas modifier)
├── area.cpp/area.h   # Fichiers du projet
└── DevisMaker.vcxproj  # Fichier projet Visual Studio
```

## Workflow Git - TRÈS IMPORTANT
- **TOUJOURS créer une branche de travail** nommée selon la fonctionnalité
- **Format du nom de branche** : `feature/[nom-descriptif-court]`
  - Exemple : `feature/calculateur-devis`
  - Exemple : `feature/export-pdf`
  - Exemple : `fix/correction-calcul-tva`
- **JAMAIS travailler directement sur main** - c'est ma version stable et sécurisée
- Avant de créer une branche, vérifie qu'on est bien sur main avec `git branch`
- Fais des commits réguliers avec des messages clairs en français
- À la fin de ton travail :
  1. Fais un dernier commit récapitulatif
  2. Dis-moi clairement : "J'ai terminé. La branche [nom] est prête à être vérifiée."
  3. **N'effectue JAMAIS de merge automatiquement** - je déciderai si je veux merger dans main
- Format des messages de commit : "Ajout de [fonctionnalité]" ou "Correction de [bug]"

## Conventions de code C++
- **Style de nommage** :
  - Variables : camelCase (ex: `monCompteur`, `prixTotal`)
  - Fonctions : camelCase (ex: `calculerSomme()`, `exporterPdf()`)
  - Classes : PascalCase (ex: `MaClasse`, `DevisCalculator`)
  - Constantes : UPPER_SNAKE_CASE (ex: `MAX_VALUE`, `TVA_TAUX`)

- **Formatage** :
  - Indentation : 4 espaces (pas de tabs)
  - Accolades : Allman style (accolade sur nouvelle ligne)
```cpp
    if (condition) 
    {
        // code
    }
```
  - Initialisation uniforme (modern C++) :
```cpp
    int x{3};
    std::string nom{"Claude"};
```

- **Commentaires** :
  - Ajoute des commentaires explicatifs pour m'aider à comprendre
  - Explique le "pourquoi" pas juste le "quoi"
  - Pour les fonctions complexes, ajoute une description de ce qu'elle fait

## Environnement de développement
- **IDE** : Visual Studio 2022 (MSVC compiler)
- **Standard C++** : C++17 ou plus récent
- Je compile via Visual Studio, pas en ligne de commande
- Je vérifie tes changements dans Visual Studio en switchant vers ta branche
- **Ne génère PAS de commandes de compilation** - Visual Studio s'en occupe

## Ce que j'attends de toi
- **Explique ton code** : Comme je suis débutant, explique pourquoi tu utilises certaines techniques
- **Utilise des analogies** : J'apprends mieux avec des comparaisons concrètes du monde réel
- **Vérifie la syntaxe** : Assure-toi que le code est du C++ valide et compile
- **Sois pédagogique** : Si tu utilises une fonctionnalité C++ avancée, explique-la simplement
- **Sécurité d'abord** : Travaille toujours sur une branche séparée avec un nom descriptif
- **Principes SOLID** : Respecte les principes de conception orientée objet
- **Pas d'over-engineering** : Reste simple si la situation ne nécessite pas de pattern complexe

## Ce que tu NE dois PAS faire
- **JAMAIS** travailler directement sur la branche `main`
- **JAMAIS** faire de merge automatiquement - je décide quand merger
- N'utilise pas de fonctionnalités C++ trop avancées sans expliquer clairement
- Ne modifie pas les fichiers de build (dossier x64/)
- Ne modifie pas les fichiers de configuration Visual Studio (.vcxproj, .filters, etc.) sauf si explicitement demandé
- Ne crée pas de branches avec des noms génériques comme "claude-temp" - utilise des noms descriptifs

## Workflow type attendu
Quand je te demande d'ajouter une fonctionnalité :
1. Tu crées une branche `feature/[nom-descriptif]` depuis main
2. Tu m'expliques ton plan d'action avant de coder
3. Tu codes avec des commits réguliers
4. Tu testes que la syntaxe est correcte
5. Tu me dis que c'est terminé et prêt à vérifier
6. J'ouvre Visual Studio, je switch vers ta branche, je compile et teste
7. Si tout est ok, je merge moi-même dans main
```