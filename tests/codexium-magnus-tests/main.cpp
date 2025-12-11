#include <QtTest/QtTest>
#include <QApplication>
#include <QDebug>

// Include test class headers
#include "Services/SignatureServiceTests.h"
#include "Services/LinkServiceTests.h"
#include "Services/CartridgeServiceTests.h"
#include "Services/SearchServiceTests.h"
#include "UI/TypographySettingsWidgetTests.h"
#include "UI/BibliographySettingsWidgetTests.h"
#include "UI/SettingsDialogTests.h"
#include "Theme/ThemeManagerTests.h"

int main(int argc, char *argv[])
{
    // Create a single QApplication for all tests
    QApplication app(argc, argv);
    
    int totalFailures = 0;
    int totalTests = 0;
    
    qDebug() << "\n========================================";
    qDebug() << "Codexium Magnus Test Suite";
    qDebug() << "========================================\n";
    
    // Run each test suite
    {
        SignatureServiceTests test;
        qDebug() << "\n=== Running SignatureServiceTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ SignatureServiceTests FAILED";
        } else {
            qDebug() << "✓ SignatureServiceTests PASSED";
        }
    }
    
    {
        LinkServiceTests test;
        qDebug() << "\n=== Running LinkServiceTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ LinkServiceTests FAILED";
        } else {
            qDebug() << "✓ LinkServiceTests PASSED";
        }
    }
    
    {
        CartridgeServiceTests test;
        qDebug() << "\n=== Running CartridgeServiceTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ CartridgeServiceTests FAILED";
        } else {
            qDebug() << "✓ CartridgeServiceTests PASSED";
        }
    }
    
    {
        SearchServiceTests test;
        qDebug() << "\n=== Running SearchServiceTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ SearchServiceTests FAILED";
        } else {
            qDebug() << "✓ SearchServiceTests PASSED";
        }
    }
    
    {
        TypographySettingsWidgetTests test;
        qDebug() << "\n=== Running TypographySettingsWidgetTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ TypographySettingsWidgetTests FAILED";
        } else {
            qDebug() << "✓ TypographySettingsWidgetTests PASSED";
        }
    }
    
    {
        BibliographySettingsWidgetTests test;
        qDebug() << "\n=== Running BibliographySettingsWidgetTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ BibliographySettingsWidgetTests FAILED";
        } else {
            qDebug() << "✓ BibliographySettingsWidgetTests PASSED";
        }
    }
    
    {
        SettingsDialogTests test;
        qDebug() << "\n=== Running SettingsDialogTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ SettingsDialogTests FAILED";
        } else {
            qDebug() << "✓ SettingsDialogTests PASSED";
        }
    }
    
    {
        ThemeManagerTests test;
        qDebug() << "\n=== Running ThemeManagerTests ===";
        int result = QTest::qExec(&test, argc, argv);
        totalTests++;
        if (result != 0) {
            totalFailures++;
            qDebug() << "✗ ThemeManagerTests FAILED";
        } else {
            qDebug() << "✓ ThemeManagerTests PASSED";
        }
    }
    
    // Summary
    qDebug() << "\n========================================";
    qDebug() << "Test Summary";
    qDebug() << "========================================";
    qDebug() << "Total Test Suites:" << totalTests;
    qDebug() << "Passed:" << (totalTests - totalFailures);
    qDebug() << "Failed:" << totalFailures;
    qDebug() << "========================================\n";
    
    return totalFailures > 0 ? 1 : 0;
}
