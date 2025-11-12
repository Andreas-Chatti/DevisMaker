# Tests Unitaires - DevisMaker

## Structure des tests

```
tests/
├── AreaTest.h              # Tests pour la classe Area
├── AreaTest.cpp
├── MovingObjectTest.h      # Tests pour la classe MovingObject
├── MovingObjectTest.cpp
├── tests.pro               # Fichier projet Qt
└── README.md               # Ce fichier
```

## Exécuter les tests

### Option 1 : Avec Qt Creator

1. Ouvrir `tests.pro` dans Qt Creator
2. Configurer le projet (choisir le kit approprié)
3. Build → Run
4. Les résultats s'affichent dans la console

### Option 2 : En ligne de commande

```bash
cd tests
qmake tests.pro
nmake       # Windows avec MSVC
# ou make  # Linux/Mac

# Exécuter les tests
./bin/tests
```

## Structure d'un test

Chaque test suit le pattern **AAA** :

```cpp
void testFunction()
{
    // ARRANGE - Préparer les données
    MovingObject obj("chaise", 0.5, "salon", 5);

    // ACT - Exécuter l'action
    obj.add(3);

    // ASSERT - Vérifier le résultat
    QCOMPARE(obj.getQuantity(), 8);
}
```

## Tests disponibles

### AreaTest (30+ tests)

- **Constructeurs** : Default, avec paramètres
- **addObject** : Ajout simple, fusion d'objets identiques, mise à jour volume
- **removeObject** : Suppression complète, objet inexistant
- **removeObject(quantity)** : Suppression partielle, exacte, excédentaire, quantités invalides
- **modifyObject** : Modification, null pointer, mise à jour volume
- **findObject** : Recherche existante/inexistante
- **Invariants** : Volume total cohérent, compteur d'objets correct

### MovingObjectTest (30+ tests)

- **Constructeurs** : Default, paramétré, copie, move
- **Opérateurs d'assignation** : Copie, move, auto-assignation
- **add()** : Incrémentation simple, mise à jour volume
- **add(quantity)** : Ajout multiple
- **remove()** : Décrémentation, cas limite à zéro
- **remove(quantity)** : Suppression partielle/totale/excédentaire
- **Setters** : Mise à jour volume unitaire/quantité/nom/area
- **Opérateur ==** : Égalité par nom et volume
- **Invariants** : TotalVolume = UnitaryVolume × Quantity

## Macros Qt Test

| Macro | Usage |
|-------|-------|
| `QVERIFY(condition)` | Vérifie qu'une condition est vraie |
| `QCOMPARE(actual, expected)` | Compare deux valeurs (affiche les différences) |
| `QVERIFY2(condition, msg)` | QVERIFY avec message personnalisé |
| `QFAIL(message)` | Fait échouer le test avec un message |
| `QTest::ignoreMessage(type, pattern)` | Ignore un message de log attendu |

## Exemple de sortie

```
********* Start testing of AreaTest *********
Config: Using QtTest library
PASS   : AreaTest::initTestCase()
PASS   : AreaTest::testConstructor_DefaultValues()
PASS   : AreaTest::testAddObject_NewObject()
PASS   : AreaTest::testRemoveObjectQuantity_PartialRemoval()
...
PASS   : AreaTest::cleanupTestCase()
Totals: 32 passed, 0 failed, 0 skipped
********* Finished testing of AreaTest *********
```

## Ajouter de nouveaux tests

1. Créer une classe héritant de `QObject`
2. Ajouter `Q_OBJECT` macro
3. Créer des slots privés avec le préfixe `test*`
4. Utiliser `QTEST_MAIN(YourTestClass)` ou `QTEST_APPLESS_MAIN`
5. Inclure le `.moc` généré : `#include "YourTest.moc"`

Exemple :

```cpp
#include <QObject>
#include <QtTest/QtTest>

class MyTest : public QObject
{
    Q_OBJECT
private slots:
    void testSomething()
    {
        QCOMPARE(2 + 2, 4);
    }
};

QTEST_APPLESS_MAIN(MyTest)
#include "MyTest.moc"
```

## Bonnes pratiques

✅ **Un test = un cas** - Ne pas tester plusieurs choses dans un seul test
✅ **Noms explicites** - `testRemoveObject_WhenQuantityExceeds_ShouldRemoveAll`
✅ **Tester les cas limites** - Zéro, négatif, max, null, vide
✅ **Tester les invariants** - Propriétés qui doivent toujours être vraies
✅ **Isoler les tests** - Chaque test doit être indépendant
✅ **Utiliser init/cleanup** - Pour réinitialiser l'état entre les tests

## Debugging des tests

```cpp
// Afficher des messages de debug
qDebug() << "Value:" << obj.getQuantity();

// Ignorer les warnings attendus
QTest::ignoreMessage(QtWarningMsg, "Expected warning message");

// Tester les exceptions (C++11+)
QVERIFY_EXCEPTION_THROWN(obj.doSomething(), std::runtime_error);
```

## Ressources

- [Qt Test Documentation](https://doc.qt.io/qt-6/qtest-overview.html)
- [Qt Test Tutorial](https://doc.qt.io/qt-6/qtest-tutorial.html)
- [Best Practices](https://wiki.qt.io/Writing_Unit_Tests)
