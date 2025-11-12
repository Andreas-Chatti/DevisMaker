#pragma once
#include <QObject>
#include <QtTest/QtTest>
#include "../src/inventory/movingObject.h"

class MovingObjectTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Constructor tests
    void testConstructor_Default();
    void testConstructor_WithParameters();
    void testConstructor_Copy();
    void testConstructor_Move();

    // Assignment operator tests
    void testCopyAssignment();
    void testMoveAssignment();
    void testCopyAssignment_SelfAssignment();
    void testMoveAssignment_SelfAssignment();

    // add() tests
    void testAdd_IncrementsByOne();
    void testAdd_UpdatesTotalVolume();

    // add(int) tests
    void testAddQuantity_AddsMultiple();
    void testAddQuantity_UpdatesTotalVolume();

    // remove() tests
    void testRemove_DecrementsByOne();
    void testRemove_WhenZero_StaysZero();
    void testRemove_UpdatesTotalVolume();

    // remove(int) tests
    void testRemoveQuantity_RemovesPartial();
    void testRemoveQuantity_RemovesExactly();
    void testRemoveQuantity_RemovesExceed();
    void testRemoveQuantity_UpdatesTotalVolume();

    // Setter tests
    void testSetUnitaryVolume_UpdatesTotalVolume();
    void testSetQuantity_UpdatesTotalVolume();
    void testSetName();
    void testSetAreaKey();

    // Equality operator test
    void testEquality_SameNameAndVolume();
    void testEquality_DifferentName();
    void testEquality_DifferentVolume();

    // Invariant tests
    void testInvariant_TotalVolumeAlwaysCorrect();
};
