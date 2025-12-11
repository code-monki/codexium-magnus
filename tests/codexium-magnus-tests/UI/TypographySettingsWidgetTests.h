#ifndef TYPOGRAPHYSETTINGSWIDGETTESTS_H
#define TYPOGRAPHYSETTINGSWIDGETTESTS_H

#include <QtTest/QtTest>

class TypographySettingsWidgetTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Configuration getter/setter tests
    void getConfig_DefaultValues_ReturnsDefaults();
    void setConfig_ValidConfig_UpdatesWidgets();
    void getConfig_AfterSetConfig_ReturnsSameValues();
    
    // Font family tests
    void getConfig_FontFamilySet_ReturnsCorrectFamily();
    
    // Font size tests
    void getConfig_FontSizeSet_ReturnsCorrectSize();
    void getConfig_FontSizeRange_RespectsLimits();
    
    // Heading scale tests
    void getConfig_HeadingScalesSet_ReturnsCorrectScales();
    void getConfig_HeadingScales_AllSixLevels();
    
    // Print options tests
    void getConfig_PrintMarginSet_ReturnsCorrectMargin();
    void getConfig_BlackOnWhiteToggled_ReturnsCorrectValue();
    
    // Integration tests
    void setConfig_CompleteConfig_AllFieldsPreserved();

private:
    void* m_widget; // TypographySettingsWidget* - using void* to avoid include in header
    int m_argc;
    char** m_argv;
    QApplication* m_app;
};

#endif // TYPOGRAPHYSETTINGSWIDGETTESTS_H
