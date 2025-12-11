#ifndef THEMEMANAGERTESTS_H
#define THEMEMANAGERTESTS_H

#include <QtTest/QtTest>

class ThemeManagerTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Theme switching tests
    void setTheme_Light_EmitsSignal();
    void setTheme_Sepia_EmitsSignal();
    void setTheme_Dark_EmitsSignal();
    void currentTheme_AfterSet_ReturnsSetTheme();
    
    // Token tests
    void getTokenMap_AfterSetTheme_ReturnsTokens();
    void getToken_ValidToken_ReturnsValue();
    void getToken_InvalidToken_ReturnsEmpty();
    
    // Custom theme tests
    void loadCustomTheme_ValidPath_SetsPath();
    void customThemePath_AfterLoad_ReturnsPath();
    
    // Palette tests
    void currentPalette_AfterSetTheme_ReturnsPalette();
    
    // Persistence tests
    void saveThemePreference_ValidTheme_Persists();
    void loadThemePreference_AfterSave_LoadsTheme();

private:
    void* m_manager; // ThemeManager* - using void* to avoid include in header
    int m_argc;
    char** m_argv;
    QApplication* m_app;
    QString m_testSettingsPath;
};

#endif // THEMEMANAGERTESTS_H
