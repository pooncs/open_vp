#include <QtTest>
#include "MathPrimitives.h"
#include "LogicPrimitives.h"
#include "DecimalData.h"
#include "BoolData.h"

class TestPrimitives : public QObject
{
    Q_OBJECT
private slots:
    // Math Tests
    void testSin();
    void testCos();
    void testAbs();
    
    // Logic Tests
    void testAnd();
    void testOr();
    void testNot();
};

void TestPrimitives::testSin()
{
    SinModel node;
    auto input = std::make_shared<DecimalData>(0.0);
    node.setInData(input, 0);
    
    auto output = std::dynamic_pointer_cast<DecimalData>(node.outData(0));
    QVERIFY(output != nullptr);
    QCOMPARE(output->number(), 0.0);
    
    // Test PI/2
    input = std::make_shared<DecimalData>(3.14159265 / 2.0);
    node.setInData(input, 0);
    output = std::dynamic_pointer_cast<DecimalData>(node.outData(0));
    QVERIFY(std::abs(output->number() - 1.0) < 0.0001);
}

void TestPrimitives::testCos()
{
    CosModel node;
    auto input = std::make_shared<DecimalData>(0.0);
    node.setInData(input, 0);
    
    auto output = std::dynamic_pointer_cast<DecimalData>(node.outData(0));
    QVERIFY(output != nullptr);
    QCOMPARE(output->number(), 1.0);
}

void TestPrimitives::testAbs()
{
    AbsModel node;
    auto input = std::make_shared<DecimalData>(-5.5);
    node.setInData(input, 0);
    
    auto output = std::dynamic_pointer_cast<DecimalData>(node.outData(0));
    QVERIFY(output != nullptr);
    QCOMPARE(output->number(), 5.5);
}

void TestPrimitives::testAnd()
{
    AndModel node;
    auto trueData = std::make_shared<BoolData>(true);
    auto falseData = std::make_shared<BoolData>(false);
    
    // True AND True
    node.setInData(trueData, 0);
    node.setInData(trueData, 1);
    auto output = std::dynamic_pointer_cast<BoolData>(node.outData(0));
    QCOMPARE(output->value(), true);
    
    // True AND False
    node.setInData(falseData, 1);
    output = std::dynamic_pointer_cast<BoolData>(node.outData(0));
    QCOMPARE(output->value(), false);
}

void TestPrimitives::testOr()
{
    OrModel node;
    auto trueData = std::make_shared<BoolData>(true);
    auto falseData = std::make_shared<BoolData>(false);
    
    // True OR False
    node.setInData(trueData, 0);
    node.setInData(falseData, 1);
    auto output = std::dynamic_pointer_cast<BoolData>(node.outData(0));
    QCOMPARE(output->value(), true);
    
    // False OR False
    node.setInData(falseData, 0);
    output = std::dynamic_pointer_cast<BoolData>(node.outData(0));
    QCOMPARE(output->value(), false);
}

void TestPrimitives::testNot()
{
    NotModel node;
    auto trueData = std::make_shared<BoolData>(true);
    
    node.setInData(trueData, 0);
    auto output = std::dynamic_pointer_cast<BoolData>(node.outData(0));
    QCOMPARE(output->value(), false);
}

QTEST_MAIN(TestPrimitives)
#include "TestPrimitives.moc"
