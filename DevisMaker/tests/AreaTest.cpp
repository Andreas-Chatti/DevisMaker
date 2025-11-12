#include "AreaTest.h"

void AreaTest::initTestCase()
{
    qDebug() << "=== Starting Area Tests ===";
}

void AreaTest::cleanupTestCase()
{
    qDebug() << "=== Area Tests Completed ===";
}

void AreaTest::init()
{
    m_testArea = new Area("TestRoom", Area::AreaType::bedroom);
}

void AreaTest::cleanup()
{
    delete m_testArea;
    m_testArea = nullptr;
}

// ========== Constructor Tests ==========

void AreaTest::testConstructor_DefaultValues()
{
    Area defaultArea;

    QCOMPARE(defaultArea.getName(), QString("UNDEFINED"));
    QCOMPARE(defaultArea.getType(), Area::AreaType::none);
    QCOMPARE(defaultArea.getTotalVolume(), 0.0);
    QCOMPARE(defaultArea.getObjectCount(), 0);
}

void AreaTest::testConstructor_WithNameAndType()
{
    Area area("Kitchen", Area::AreaType::kitchen);

    QCOMPARE(area.getName(), QString("Kitchen"));
    QCOMPARE(area.getType(), Area::AreaType::kitchen);
    QCOMPARE(area.getTotalVolume(), 0.0);
}

// ========== addObject Tests ==========

void AreaTest::testAddObject_NewObject()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);

    // ACT
    m_testArea->addObject(std::move(chair));

    // ASSERT
    QCOMPARE(m_testArea->getObjectCount(), 1);

    const MovingObject* found = m_testArea->findObject("chaise");
    QVERIFY(found != nullptr);
    QCOMPARE(found->getQuantity(), 5);
    QCOMPARE(found->getTotalVolume(), 2.5);
}

void AreaTest::testAddObject_SameObjectTwice_ShouldMergeQuantities()
{
    // ARRANGE
    MovingObject chair1("chaise", 0.5, "TestRoom", 3);
    MovingObject chair2("chaise", 0.5, "TestRoom", 2);

    // ACT
    m_testArea->addObject(std::move(chair1));
    m_testArea->addObject(std::move(chair2));

    // ASSERT
    QCOMPARE(m_testArea->getObjectCount(), 1); // Still one object

    const MovingObject* found = m_testArea->findObject("chaise");
    QVERIFY(found != nullptr);
    QCOMPARE(found->getQuantity(), 5); // 3 + 2 = 5
    QCOMPARE(found->getTotalVolume(), 2.5); // 0.5 * 5
}

void AreaTest::testAddObject_UpdatesTotalVolume()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 4);
    MovingObject table("table", 1.2, "TestRoom", 2);

    // ACT
    m_testArea->addObject(std::move(chair));
    QCOMPARE(m_testArea->getTotalVolume(), 2.0); // 0.5 * 4

    m_testArea->addObject(std::move(table));

    // ASSERT
    QCOMPARE(m_testArea->getTotalVolume(), 4.4); // 2.0 + 2.4
}

// ========== removeObject(QString) Tests ==========

void AreaTest::testRemoveObject_ExistingObject()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    m_testArea->removeObject("chaise");

    // ASSERT
    QVERIFY(m_testArea->findObject("chaise") == nullptr);
    QCOMPARE(m_testArea->getObjectCount(), 0);
}

void AreaTest::testRemoveObject_NonExistentObject()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT (should not crash)
    m_testArea->removeObject("table");

    // ASSERT
    QCOMPARE(m_testArea->getObjectCount(), 1); // Chair still there
}

void AreaTest::testRemoveObject_UpdatesTotalVolume()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 4);
    MovingObject table("table", 1.2, "TestRoom", 2);
    m_testArea->addObject(std::move(chair));
    m_testArea->addObject(std::move(table));

    double volumeBefore = m_testArea->getTotalVolume(); // 2.0 + 2.4 = 4.4

    // ACT
    m_testArea->removeObject("chaise");

    // ASSERT
    QCOMPARE(m_testArea->getTotalVolume(), volumeBefore - 2.0);
}

// ========== removeObject(QString, int) Tests ==========

void AreaTest::testRemoveObjectQuantity_PartialRemoval()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 10);
    m_testArea->addObject(std::move(chair));

    // ACT
    m_testArea->removeObject("chaise", 3);

    // ASSERT
    const MovingObject* found = m_testArea->findObject("chaise");
    QVERIFY(found != nullptr);
    QCOMPARE(found->getQuantity(), 7); // 10 - 3
    QCOMPARE(found->getTotalVolume(), 3.5); // 0.5 * 7
    QCOMPARE(m_testArea->getTotalVolume(), 3.5);
}

void AreaTest::testRemoveObjectQuantity_ExactQuantity()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    m_testArea->removeObject("chaise", 5);

    // ASSERT
    QVERIFY(m_testArea->findObject("chaise") == nullptr);
    QCOMPARE(m_testArea->getTotalVolume(), 0.0);
}

void AreaTest::testRemoveObjectQuantity_ExceedQuantity()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    m_testArea->removeObject("chaise", 100); // Way more than available

    // ASSERT
    QVERIFY(m_testArea->findObject("chaise") == nullptr);
    QCOMPARE(m_testArea->getTotalVolume(), 0.0);
}

void AreaTest::testRemoveObjectQuantity_ZeroQuantity()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    QTest::ignoreMessage(QtWarningMsg, "[Area::removeObject] Quantity arg was <= 0");
    m_testArea->removeObject("chaise", 0);

    // ASSERT
    const MovingObject* found = m_testArea->findObject("chaise");
    QVERIFY(found != nullptr);
    QCOMPARE(found->getQuantity(), 5); // Unchanged
}

void AreaTest::testRemoveObjectQuantity_NegativeQuantity()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    QTest::ignoreMessage(QtWarningMsg, "[Area::removeObject] Quantity arg was <= 0");
    m_testArea->removeObject("chaise", -3);

    // ASSERT
    QCOMPARE(m_testArea->findObject("chaise")->getQuantity(), 5);
}

void AreaTest::testRemoveObjectQuantity_NonExistentObject()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Object .* doesn't exist"));
    m_testArea->removeObject("table", 3);

    // ASSERT
    QCOMPARE(m_testArea->getObjectCount(), 1); // Chair still there
}

// ========== modifyObject Tests ==========

void AreaTest::testModifyObject_UpdatesExistingObject()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    const MovingObject* oldChair = m_testArea->findObject("chaise");

    // ACT
    MovingObject newChair("chaise", 0.6, "TestRoom", 8); // Different volume and quantity
    m_testArea->modifyObject(oldChair, std::move(newChair));

    // ASSERT
    const MovingObject* found = m_testArea->findObject("chaise");
    QVERIFY(found != nullptr);
    QCOMPARE(found->getUnitaryVolume(), 0.6);
    QCOMPARE(found->getQuantity(), 8);
    QCOMPARE(found->getTotalVolume(), 4.8); // 0.6 * 8
}

void AreaTest::testModifyObject_NullPointer_ShouldDoNothing()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    MovingObject newObject("table", 1.0, "TestRoom", 1);
    m_testArea->modifyObject(nullptr, std::move(newObject));

    // ASSERT
    QCOMPARE(m_testArea->getObjectCount(), 1); // Still just the chair
    QVERIFY(m_testArea->findObject("table") == nullptr);
}

void AreaTest::testModifyObject_NonExistentObject_ShouldDoNothing()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    MovingObject fakeObject("nonexistent", 1.0, "TestRoom", 1);
    const MovingObject* fakePtr = &fakeObject;

    // ACT
    MovingObject newObject("table", 1.0, "TestRoom", 1);
    m_testArea->modifyObject(fakePtr, std::move(newObject));

    // ASSERT
    QCOMPARE(m_testArea->getObjectCount(), 1);
    QVERIFY(m_testArea->findObject("table") == nullptr);
}

void AreaTest::testModifyObject_UpdatesTotalVolume()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 10); // Volume = 5.0
    m_testArea->addObject(std::move(chair));

    const MovingObject* oldChair = m_testArea->findObject("chaise");

    // ACT
    MovingObject newChair("chaise", 1.0, "TestRoom", 3); // Volume = 3.0
    m_testArea->modifyObject(oldChair, std::move(newChair));

    // ASSERT
    QCOMPARE(m_testArea->getTotalVolume(), 3.0);
}

// ========== findObject Tests ==========

void AreaTest::testFindObject_ExistingObject()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));

    // ACT
    const MovingObject* found = m_testArea->findObject("chaise");

    // ASSERT
    QVERIFY(found != nullptr);
    QCOMPARE(found->getName(), QString("chaise"));
}

void AreaTest::testFindObject_NonExistentObject()
{
    // ACT
    const MovingObject* found = m_testArea->findObject("nonexistent");

    // ASSERT
    QVERIFY(found == nullptr);
}

// ========== updateObjectsAreaKey Tests ==========

void AreaTest::testUpdateObjectsAreaKey_UpdatesAllObjects()
{
    // ARRANGE
    MovingObject chair("chaise", 0.5, "TestRoom", 3);
    MovingObject table("table", 1.2, "TestRoom", 2);
    m_testArea->addObject(std::move(chair));
    m_testArea->addObject(std::move(table));

    // ACT
    m_testArea->updateObjectsAreaKey("NewRoom");

    // ASSERT
    const MovingObject* foundChair = m_testArea->findObject("chaise");
    const MovingObject* foundTable = m_testArea->findObject("table");

    QCOMPARE(foundChair->getAreaKey(), QString("NewRoom"));
    QCOMPARE(foundTable->getAreaKey(), QString("NewRoom"));
}

// ========== Invariant Tests ==========

void AreaTest::testInvariant_TotalVolumeAlwaysConsistent()
{
    // Add objects
    MovingObject chair("chaise", 0.5, "TestRoom", 10); // 5.0
    MovingObject table("table", 1.2, "TestRoom", 3);   // 3.6
    m_testArea->addObject(std::move(chair));
    m_testArea->addObject(std::move(table));

    QCOMPARE(m_testArea->getTotalVolume(), 8.6);

    // Remove partial
    m_testArea->removeObject("chaise", 3); // 5.0 - 1.5 = 3.5
    QCOMPARE(m_testArea->getTotalVolume(), 7.1); // 3.5 + 3.6

    // Modify
    const MovingObject* oldTable = m_testArea->findObject("table");
    MovingObject newTable("table", 2.0, "TestRoom", 2); // 4.0
    m_testArea->modifyObject(oldTable, std::move(newTable));
    QCOMPARE(m_testArea->getTotalVolume(), 7.5); // 3.5 + 4.0

    // Remove all
    m_testArea->removeObject("chaise");
    m_testArea->removeObject("table");
    QCOMPARE(m_testArea->getTotalVolume(), 0.0);
}

void AreaTest::testInvariant_ObjectCountMatchesHashSize()
{
    QCOMPARE(m_testArea->getObjectCount(), 0);

    MovingObject chair("chaise", 0.5, "TestRoom", 5);
    m_testArea->addObject(std::move(chair));
    QCOMPARE(m_testArea->getObjectCount(), 1);

    MovingObject table("table", 1.2, "TestRoom", 2);
    m_testArea->addObject(std::move(table));
    QCOMPARE(m_testArea->getObjectCount(), 2);

    m_testArea->removeObject("chaise");
    QCOMPARE(m_testArea->getObjectCount(), 1);

    m_testArea->removeObject("table");
    QCOMPARE(m_testArea->getObjectCount(), 0);
}

QTEST_MAIN(AreaTest)
#include "AreaTest.moc"
