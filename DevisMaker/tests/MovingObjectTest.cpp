#include "MovingObjectTest.h"

void MovingObjectTest::initTestCase()
{
    qDebug() << "=== Starting MovingObject Tests ===";
}

void MovingObjectTest::cleanupTestCase()
{
    qDebug() << "=== MovingObject Tests Completed ===";
}

// ========== Constructor Tests ==========

void MovingObjectTest::testConstructor_Default()
{
    MovingObject obj;

    QCOMPARE(obj.getName(), QString("UNDEFINED"));
    QCOMPARE(obj.getUnitaryVolume(), 0.0);
    QCOMPARE(obj.getQuantity(), 0);
    QCOMPARE(obj.getTotalVolume(), 0.0);
    QCOMPARE(obj.getAreaKey(), QString("default"));
    QCOMPARE(obj.isDisassembly(), false);
    QCOMPARE(obj.isAssembly(), false);
    QCOMPARE(obj.isHeavy(), false);
}

void MovingObjectTest::testConstructor_WithParameters()
{
    MovingObject obj("chaise", 0.5, "salon", 10, true, false, true, "Fragile");

    QCOMPARE(obj.getName(), QString("chaise"));
    QCOMPARE(obj.getUnitaryVolume(), 0.5);
    QCOMPARE(obj.getQuantity(), 10);
    QCOMPARE(obj.getTotalVolume(), 5.0); // 0.5 * 10
    QCOMPARE(obj.getAreaKey(), QString("salon"));
    QCOMPARE(obj.isDisassembly(), true);
    QCOMPARE(obj.isAssembly(), false);
    QCOMPARE(obj.isHeavy(), true);
    QCOMPARE(obj.getNote(), QString("Fragile"));
}

void MovingObjectTest::testConstructor_Copy()
{
    MovingObject original("table", 1.2, "cuisine", 3);
    MovingObject copy(original);

    QCOMPARE(copy.getName(), original.getName());
    QCOMPARE(copy.getUnitaryVolume(), original.getUnitaryVolume());
    QCOMPARE(copy.getQuantity(), original.getQuantity());
    QCOMPARE(copy.getTotalVolume(), original.getTotalVolume());
    QCOMPARE(copy.getAreaKey(), original.getAreaKey());
}

void MovingObjectTest::testConstructor_Move()
{
    MovingObject original("chaise", 0.5, "salon", 5);
    QString originalName = original.getName();
    double originalVolume = original.getUnitaryVolume();

    MovingObject moved(std::move(original));

    QCOMPARE(moved.getName(), originalName);
    QCOMPARE(moved.getUnitaryVolume(), originalVolume);
    QCOMPARE(moved.getQuantity(), 5);
}

// ========== Assignment Operator Tests ==========

void MovingObjectTest::testCopyAssignment()
{
    MovingObject obj1("chaise", 0.5, "salon", 3);
    MovingObject obj2;

    obj2 = obj1;

    QCOMPARE(obj2.getName(), obj1.getName());
    QCOMPARE(obj2.getUnitaryVolume(), obj1.getUnitaryVolume());
    QCOMPARE(obj2.getQuantity(), obj1.getQuantity());
    QCOMPARE(obj2.getTotalVolume(), obj1.getTotalVolume());
}

void MovingObjectTest::testMoveAssignment()
{
    MovingObject obj1("table", 1.5, "bureau", 2);
    MovingObject obj2;

    QString expectedName = obj1.getName();
    double expectedVolume = obj1.getUnitaryVolume();

    obj2 = std::move(obj1);

    QCOMPARE(obj2.getName(), expectedName);
    QCOMPARE(obj2.getUnitaryVolume(), expectedVolume);
    QCOMPARE(obj2.getQuantity(), 2);
}

void MovingObjectTest::testCopyAssignment_SelfAssignment()
{
    MovingObject obj("chaise", 0.5, "salon", 5);

    obj = obj; // Self-assignment

    QCOMPARE(obj.getName(), QString("chaise"));
    QCOMPARE(obj.getQuantity(), 5);
}

void MovingObjectTest::testMoveAssignment_SelfAssignment()
{
    MovingObject obj("table", 1.0, "cuisine", 3);

    obj = std::move(obj); // Self-assignment

    QCOMPARE(obj.getName(), QString("table"));
    QCOMPARE(obj.getQuantity(), 3);
}

// ========== add() Tests ==========

void MovingObjectTest::testAdd_IncrementsByOne()
{
    MovingObject obj("chaise", 0.5, "salon", 5);

    int result = obj.add();

    QCOMPARE(result, 6);
    QCOMPARE(obj.getQuantity(), 6);
}

void MovingObjectTest::testAdd_UpdatesTotalVolume()
{
    MovingObject obj("chaise", 0.5, "salon", 5); // Volume = 2.5

    obj.add();

    QCOMPARE(obj.getTotalVolume(), 3.0); // 0.5 * 6
}

// ========== add(int) Tests ==========

void MovingObjectTest::testAddQuantity_AddsMultiple()
{
    MovingObject obj("table", 1.2, "bureau", 3);

    int result = obj.add(5);

    QCOMPARE(result, 8);
    QCOMPARE(obj.getQuantity(), 8);
}

void MovingObjectTest::testAddQuantity_UpdatesTotalVolume()
{
    MovingObject obj("chaise", 0.5, "salon", 10); // Volume = 5.0

    obj.add(4);

    QCOMPARE(obj.getTotalVolume(), 7.0); // 0.5 * 14
}

// ========== remove() Tests ==========

void MovingObjectTest::testRemove_DecrementsByOne()
{
    MovingObject obj("chaise", 0.5, "salon", 5);

    int result = obj.remove();

    QCOMPARE(result, 4);
    QCOMPARE(obj.getQuantity(), 4);
}

void MovingObjectTest::testRemove_WhenZero_StaysZero()
{
    MovingObject obj("chaise", 0.5, "salon", 0);

    int result = obj.remove();

    QCOMPARE(result, 0);
    QCOMPARE(obj.getQuantity(), 0);
}

void MovingObjectTest::testRemove_UpdatesTotalVolume()
{
    MovingObject obj("table", 1.0, "cuisine", 5); // Volume = 5.0

    obj.remove();

    QCOMPARE(obj.getTotalVolume(), 4.0); // 1.0 * 4
}

// ========== remove(int) Tests ==========

void MovingObjectTest::testRemoveQuantity_RemovesPartial()
{
    MovingObject obj("chaise", 0.5, "salon", 10);

    int result = obj.remove(3);

    QCOMPARE(result, 7);
    QCOMPARE(obj.getQuantity(), 7);
}

void MovingObjectTest::testRemoveQuantity_RemovesExactly()
{
    MovingObject obj("table", 1.2, "bureau", 5);

    int result = obj.remove(5);

    QCOMPARE(result, 0);
    QCOMPARE(obj.getQuantity(), 0);
    QCOMPARE(obj.getTotalVolume(), 0.0);
}

void MovingObjectTest::testRemoveQuantity_RemovesExceed()
{
    MovingObject obj("chaise", 0.5, "salon", 3);

    int result = obj.remove(10); // More than available

    QCOMPARE(result, 0);
    QCOMPARE(obj.getQuantity(), 0);
    QCOMPARE(obj.getTotalVolume(), 0.0);
}

void MovingObjectTest::testRemoveQuantity_UpdatesTotalVolume()
{
    MovingObject obj("table", 2.0, "salon", 10); // Volume = 20.0

    obj.remove(4);

    QCOMPARE(obj.getTotalVolume(), 12.0); // 2.0 * 6
}

// ========== Setter Tests ==========

void MovingObjectTest::testSetUnitaryVolume_UpdatesTotalVolume()
{
    MovingObject obj("chaise", 0.5, "salon", 10); // Volume = 5.0

    obj.setUnitaryVolume(1.0);

    QCOMPARE(obj.getUnitaryVolume(), 1.0);
    QCOMPARE(obj.getTotalVolume(), 10.0); // 1.0 * 10
}

void MovingObjectTest::testSetQuantity_UpdatesTotalVolume()
{
    MovingObject obj("table", 1.5, "bureau", 5); // Volume = 7.5

    obj.setQuantity(3);

    QCOMPARE(obj.getQuantity(), 3);
    QCOMPARE(obj.getTotalVolume(), 4.5); // 1.5 * 3
}

void MovingObjectTest::testSetName()
{
    MovingObject obj("chaise", 0.5, "salon");

    obj.setName("table");

    QCOMPARE(obj.getName(), QString("table"));
}

void MovingObjectTest::testSetAreaKey()
{
    MovingObject obj("chaise", 0.5, "salon");

    obj.setAreaKey("cuisine");

    QCOMPARE(obj.getAreaKey(), QString("cuisine"));
}

// ========== Equality Operator Tests ==========

void MovingObjectTest::testEquality_SameNameAndVolume()
{
    MovingObject obj1("chaise", 0.5, "salon", 5);
    MovingObject obj2("chaise", 0.5, "cuisine", 10); // Different area and quantity

    QVERIFY(obj1 == obj2); // Should be equal (only name and volume matter)
}

void MovingObjectTest::testEquality_DifferentName()
{
    MovingObject obj1("chaise", 0.5, "salon");
    MovingObject obj2("table", 0.5, "salon");

    QVERIFY(!(obj1 == obj2));
}

void MovingObjectTest::testEquality_DifferentVolume()
{
    MovingObject obj1("chaise", 0.5, "salon");
    MovingObject obj2("chaise", 1.0, "salon");

    QVERIFY(!(obj1 == obj2));
}

// ========== Invariant Tests ==========

void MovingObjectTest::testInvariant_TotalVolumeAlwaysCorrect()
{
    MovingObject obj("chaise", 0.5, "salon", 10);

    // Initial
    QCOMPARE(obj.getTotalVolume(), 5.0);

    // After add
    obj.add(5);
    QCOMPARE(obj.getTotalVolume(), obj.getUnitaryVolume() * obj.getQuantity());

    // After remove
    obj.remove(3);
    QCOMPARE(obj.getTotalVolume(), obj.getUnitaryVolume() * obj.getQuantity());

    // After setQuantity
    obj.setQuantity(20);
    QCOMPARE(obj.getTotalVolume(), obj.getUnitaryVolume() * obj.getQuantity());

    // After setUnitaryVolume
    obj.setUnitaryVolume(1.5);
    QCOMPARE(obj.getTotalVolume(), obj.getUnitaryVolume() * obj.getQuantity());
}

QTEST_APPLESS_MAIN(MovingObjectTest)
#include "MovingObjectTest.moc"
