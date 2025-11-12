#pragma once
#include <QObject>
#include <QtTest/QtTest>
#include "../area.h"
#include "../src/inventory/movingObject.h"

class AreaTest : public QObject
{
    Q_OBJECT

private slots:
    // Setup and teardown
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testConstructor_DefaultValues();
    void testConstructor_WithNameAndType();

    // addObject tests
    void testAddObject_NewObject();
    void testAddObject_SameObjectTwice_ShouldMergeQuantities();
    void testAddObject_UpdatesTotalVolume();

    // removeObject(QString) tests
    void testRemoveObject_ExistingObject();
    void testRemoveObject_NonExistentObject();
    void testRemoveObject_UpdatesTotalVolume();

    // removeObject(QString, int) tests
    void testRemoveObjectQuantity_PartialRemoval();
    void testRemoveObjectQuantity_ExactQuantity();
    void testRemoveObjectQuantity_ExceedQuantity();
    void testRemoveObjectQuantity_ZeroQuantity();
    void testRemoveObjectQuantity_NegativeQuantity();
    void testRemoveObjectQuantity_NonExistentObject();

    // modifyObject tests
    void testModifyObject_UpdatesExistingObject();
    void testModifyObject_NullPointer_ShouldDoNothing();
    void testModifyObject_NonExistentObject_ShouldDoNothing();
    void testModifyObject_UpdatesTotalVolume();

    // findObject tests
    void testFindObject_ExistingObject();
    void testFindObject_NonExistentObject();

    // updateObjectsAreaKey tests
    void testUpdateObjectsAreaKey_UpdatesAllObjects();

    // Invariant tests
    void testInvariant_TotalVolumeAlwaysConsistent();
    void testInvariant_ObjectCountMatchesHashSize();

private:
    Area* m_testArea;
};
