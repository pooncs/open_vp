#include <QtTest>
#include "MathPrimitives.h"
#include "MathNodes.h"
#include "ComplexData.h"
#include "ArrayNodes.h"
#include "ClusterNodes.h"
#include "DecimalData.h"
#include <vector>

class TestComplexData : public QObject
{
    Q_OBJECT
private slots:
    void testArrayCreation();
    void testClusterCreation();
    void testPolymorphicAddition();
};

void TestComplexData::testArrayCreation()
{
    BuildArrayNodeModel buildArray;
    auto d1 = std::make_shared<DecimalData>(1.0);
    auto d2 = std::make_shared<DecimalData>(2.0);

    buildArray.setInData(d1, 0);
    buildArray.setInData(d2, 1);

    auto output = buildArray.outData(0);
    QVERIFY(output != nullptr);
    QCOMPARE(output->type().id, "array");

    auto arrayData = std::dynamic_pointer_cast<ArrayData>(output);
    QCOMPARE(arrayData->elements().size(), 2);
    
    auto e1 = std::dynamic_pointer_cast<DecimalData>(arrayData->elements()[0]);
    QCOMPARE(e1->number(), 1.0);
}

void TestComplexData::testClusterCreation()
{
    BundleNodeModel bundle;
    auto d1 = std::make_shared<DecimalData>(10.0);
    auto d2 = std::make_shared<DecimalData>(20.0);

    bundle.setInData(d1, 0);
    bundle.setInData(d2, 1);

    auto output = bundle.outData(0);
    QVERIFY(output != nullptr);
    QCOMPARE(output->type().id, "cluster");

    auto clusterData = std::dynamic_pointer_cast<ClusterData>(output);
    QCOMPARE(clusterData->elements().size(), 2);
}

void TestComplexData::testPolymorphicAddition()
{
    // 1. Scalar + Scalar
    AdditionModel add;
    add.setInData(std::make_shared<DecimalData>(5.0), 0);
    add.setInData(std::make_shared<DecimalData>(3.0), 1);
    
    auto outScalar = std::dynamic_pointer_cast<DecimalData>(add.outData(0));
    QCOMPARE(outScalar->number(), 8.0);

    // 2. Scalar + Array ([1, 2]) -> [6, 7]
    std::vector<std::shared_ptr<QtNodes::NodeData>> vec;
    vec.push_back(std::make_shared<DecimalData>(1.0));
    vec.push_back(std::make_shared<DecimalData>(2.0));
    auto arrayData = std::make_shared<ArrayData>(vec, DecimalData().type());

    add.setInData(std::make_shared<DecimalData>(5.0), 0);
    add.setInData(arrayData, 1);

    auto outArray = std::dynamic_pointer_cast<ArrayData>(add.outData(0));
    QVERIFY(outArray != nullptr);
    QCOMPARE(outArray->elements().size(), 2);
    
    auto e0 = std::dynamic_pointer_cast<DecimalData>(outArray->elements()[0]);
    QCOMPARE(e0->number(), 6.0);
}

QTEST_MAIN(TestComplexData)
#include "TestComplexData.moc"
