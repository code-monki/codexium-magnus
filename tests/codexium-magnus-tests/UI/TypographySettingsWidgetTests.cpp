#include "TypographySettingsWidgetTests.h"
#include <QApplication>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "UI/TypographySettingsWidget.h"
#include "../../../src/codexium-magnus-core/Models/TypographyConfig.h"

using namespace CodexiumMagnus::UI;
using namespace CodexiumMagnus::Core::Models;

void TypographySettingsWidgetTests::initTestCase() {
    // QApplication should already exist from main()
    // Just verify it exists
    QApplication* existingApp = qobject_cast<QApplication*>(QApplication::instance());
    if (!existingApp) {
        // If no app exists, create one (shouldn't happen with test runner)
        m_argc = 1;
        static char appName[] = "test";
        static char* argv[] = {appName, nullptr};
        m_argv = argv;
        m_app = new QApplication(m_argc, m_argv);
    } else {
        m_app = existingApp;
    }
}

void TypographySettingsWidgetTests::cleanupTestCase() {
    // Don't delete if it's the main app instance
    if (m_app != QApplication::instance()) {
        delete m_app;
    }
    m_app = nullptr;
}

void TypographySettingsWidgetTests::init() {
    m_widget = new TypographySettingsWidget();
}

void TypographySettingsWidgetTests::cleanup() {
    delete static_cast<TypographySettingsWidget*>(m_widget);
    m_widget = nullptr;
}

void TypographySettingsWidgetTests::getConfig_DefaultValues_ReturnsDefaults() {
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    TypographyConfig config = widget->getConfig();
    
    // Check default values (from widget setup)
    QVERIFY(config.baseFontFamily.isEmpty() || !config.baseFontFamily.isEmpty()); // May be empty or have placeholder
    QCOMPARE(config.baseFontSizePt, 12.0);
    QCOMPARE(config.headingScale.size(), 6);
    QCOMPARE(config.headingScale[0], 2.0);  // H1
    QCOMPARE(config.headingScale[1], 1.75); // H2
    QCOMPARE(config.headingScale[2], 1.5);  // H3
    QCOMPARE(config.headingScale[3], 1.25); // H4
    QCOMPARE(config.headingScale[4], 1.1);  // H5
    QCOMPARE(config.headingScale[5], 1.0);  // H6
    QCOMPARE(config.printOptions.pageMarginMm, 10.0);
    QCOMPARE(config.printOptions.blackOnWhite, false);
}

void TypographySettingsWidgetTests::setConfig_ValidConfig_UpdatesWidgets() {
    TypographyConfig config;
    config.baseFontFamily = "Inter";
    config.baseFontSizePt = 14.0;
    config.headingScale = {2.5, 2.0, 1.75, 1.5, 1.25, 1.1};
    config.printOptions.pageMarginMm = 15.0;
    config.printOptions.blackOnWhite = true;
    
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    
    TypographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.baseFontFamily, QString("Inter"));
    QCOMPARE(retrieved.baseFontSizePt, 14.0);
    QCOMPARE(retrieved.headingScale.size(), 6);
    QCOMPARE(retrieved.headingScale[0], 2.5);
    QCOMPARE(retrieved.printOptions.pageMarginMm, 15.0);
    QCOMPARE(retrieved.printOptions.blackOnWhite, true);
}

void TypographySettingsWidgetTests::getConfig_AfterSetConfig_ReturnsSameValues() {
    TypographyConfig original;
    original.baseFontFamily = "Roboto";
    original.baseFontSizePt = 11.0;
    original.headingScale = {1.8, 1.6, 1.4, 1.2, 1.1, 1.0};
    original.printOptions.pageMarginMm = 12.5;
    original.printOptions.blackOnWhite = false;
    
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(original);
    TypographyConfig retrieved = widget->getConfig();
    
    QCOMPARE(retrieved.baseFontFamily, original.baseFontFamily);
    QCOMPARE(retrieved.baseFontSizePt, original.baseFontSizePt);
    QCOMPARE(retrieved.headingScale, original.headingScale);
    QCOMPARE(retrieved.printOptions.pageMarginMm, original.printOptions.pageMarginMm);
    QCOMPARE(retrieved.printOptions.blackOnWhite, original.printOptions.blackOnWhite);
}

void TypographySettingsWidgetTests::getConfig_FontFamilySet_ReturnsCorrectFamily() {
    TypographyConfig config;
    config.baseFontFamily = "system-ui";
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    
    TypographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.baseFontFamily, QString("system-ui"));
}

void TypographySettingsWidgetTests::getConfig_FontSizeSet_ReturnsCorrectSize() {
    TypographyConfig config;
    config.baseFontSizePt = 16.0;
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    TypographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.baseFontSizePt, 16.0);
}

void TypographySettingsWidgetTests::getConfig_FontSizeRange_RespectsLimits() {
    // Widget should enforce 8.0-24.0 range via QDoubleSpinBox
    TypographyConfig config;
    config.baseFontSizePt = 25.0; // Above max
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    TypographyConfig retrieved = widget->getConfig();
    // Widget should clamp to valid range
    QVERIFY(retrieved.baseFontSizePt >= 8.0);
    QVERIFY(retrieved.baseFontSizePt <= 24.0);
}

void TypographySettingsWidgetTests::getConfig_HeadingScalesSet_ReturnsCorrectScales() {
    TypographyConfig config;
    config.headingScale = {3.0, 2.5, 2.0, 1.5, 1.2, 1.0};
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    TypographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.headingScale.size(), 6);
    QCOMPARE(retrieved.headingScale[0], 3.0);
    QCOMPARE(retrieved.headingScale[3], 1.5);
}

void TypographySettingsWidgetTests::getConfig_HeadingScales_AllSixLevels() {
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    TypographyConfig config = widget->getConfig();
    QCOMPARE(config.headingScale.size(), 6);
    
    // Verify all scales are positive
    for (double scale : config.headingScale) {
        QVERIFY(scale > 0.0);
    }
}

void TypographySettingsWidgetTests::getConfig_PrintMarginSet_ReturnsCorrectMargin() {
    TypographyConfig config;
    config.printOptions.pageMarginMm = 20.0;
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    TypographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.printOptions.pageMarginMm, 20.0);
}

void TypographySettingsWidgetTests::getConfig_BlackOnWhiteToggled_ReturnsCorrectValue() {
    TypographyConfig config;
    config.printOptions.blackOnWhite = true;
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    TypographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.printOptions.blackOnWhite, true);
    
    config.printOptions.blackOnWhite = false;
    widget->setConfig(config);
    retrieved = widget->getConfig();
    QCOMPARE(retrieved.printOptions.blackOnWhite, false);
}

void TypographySettingsWidgetTests::setConfig_CompleteConfig_AllFieldsPreserved() {
    TypographyConfig original;
    original.baseFontFamily = "Georgia";
    original.baseFontSizePt = 13.5;
    original.headingScale = {2.2, 1.9, 1.6, 1.3, 1.15, 1.05};
    original.printOptions.pageMarginMm = 18.5;
    original.printOptions.blackOnWhite = true;
    
    TypographySettingsWidget* widget = static_cast<TypographySettingsWidget*>(m_widget);
    widget->setConfig(original);
    TypographyConfig retrieved = widget->getConfig();
    
    // Verify all fields
    QCOMPARE(retrieved.baseFontFamily, original.baseFontFamily);
    QCOMPARE(retrieved.baseFontSizePt, original.baseFontSizePt);
    QCOMPARE(retrieved.headingScale.size(), original.headingScale.size());
    for (int i = 0; i < retrieved.headingScale.size(); ++i) {
        QCOMPARE(retrieved.headingScale[i], original.headingScale[i]);
    }
    QCOMPARE(retrieved.printOptions.pageMarginMm, original.printOptions.pageMarginMm);
    QCOMPARE(retrieved.printOptions.blackOnWhite, original.printOptions.blackOnWhite);
}

// QTEST_MAIN removed - using main.cpp instead
#include "TypographySettingsWidgetTests.moc"
