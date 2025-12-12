#include "PythonScriptNodeModel.h"
#include "StringData.h"
#include "ErrorData.h"
#include <QtTest>
#include <QFileInfo>
#include <QDir>

using namespace QtNodes;

class TestPythonScript : public QObject {
    Q_OBJECT
private slots:
    void testExecution();
    void testMissingFile();
};

void TestPythonScript::testExecution() {
    PythonScriptNodeModel model;
    
    // Construct path to test script
    // Assumes test is run from build directory or project root
    // We try to find the script relative to current dir
    QString scriptPath = QDir::currentPath() + "/tests/scripts/test_script.py";
    if (!QFileInfo::exists(scriptPath)) {
        // Try project root relative if we are in build dir
        scriptPath = QDir::currentPath() + "/../tests/scripts/test_script.py";
    }
    
    // If still not found, try absolute path assuming standard layout
    if (!QFileInfo::exists(scriptPath)) {
         scriptPath = "c:/Users/hmgics/projects/qtlv/tests/scripts/test_script.py";
    }
    
    QVERIFY2(QFileInfo::exists(scriptPath), "Test script not found");

    auto pathData = std::make_shared<StringData>(scriptPath);
    auto argsData = std::make_shared<StringData>("--test arg");

    // Connect signals to spy
    QSignalSpy spy(&model, &NodeDelegateModel::dataUpdated);

    // Set Input 0 (Path)
    model.setInData(pathData, 0);

    // Should emit dataUpdated for both outputs
    QCOMPARE(spy.count(), 2);
    
    // Check Output
    auto outData = model.outData(0);
    auto strOut = std::dynamic_pointer_cast<StringData>(outData);
    QVERIFY(strOut != nullptr);
    QVERIFY(strOut->value().contains("Python Script Output"));

    // Check Error
    auto errData = model.outData(1);
    auto errOut = std::dynamic_pointer_cast<ErrorData>(errData);
    QVERIFY(errOut != nullptr);
    QVERIFY(errOut->status() == false); // No error

    // Test with Args
    model.setInData(argsData, 1);
    // Trigger re-execution (since path is already set, setting args might not trigger if we don't handle it)
    // My implementation: setInData(port 1) -> updates _args -> checks _scriptPath -> executeScript()
    
    QCOMPARE(spy.count(), 4); // +2 signals
    
    outData = model.outData(0);
    strOut = std::dynamic_pointer_cast<StringData>(outData);
    QVERIFY(strOut->value().contains("Arguments"));
    QVERIFY(strOut->value().contains("--test"));
}

void TestPythonScript::testMissingFile() {
    PythonScriptNodeModel model;
    auto pathData = std::make_shared<StringData>("non_existent_script.py");
    
    model.setInData(pathData, 0);
    
    auto errData = model.outData(1);
    auto errOut = std::dynamic_pointer_cast<ErrorData>(errData);
    QVERIFY(errOut != nullptr);
    QVERIFY(errOut->status() == true); // Error
}

QTEST_MAIN(TestPythonScript)
#include "TestPythonScript.moc"
