#ifndef SETTINGSDIALOGTESTS_H
#define SETTINGSDIALOGTESTS_H

#include <QtTest/QtTest>

class SettingsDialogTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Configuration getter/setter tests
    void getTypographyConfig_DefaultValues_ReturnsDefaults();
    void getBibliographyConfig_DefaultValues_ReturnsDefaults();
    void setTypographyConfig_ValidConfig_UpdatesDialog();
    void setBibliographyConfig_ValidConfig_UpdatesDialog();
    
    // Settings persistence tests
    void loadSettings_NoSavedSettings_UsesDefaults();
    void saveSettings_ValidSettings_PersistsToQSettings();
    void loadSettings_AfterSave_LoadsSavedValues();

private:
    void* m_dialog; // SettingsDialog* - using void* to avoid include in header
    int m_argc;
    char** m_argv;
    QApplication* m_app;
    QString m_testSettingsPath;
};

#endif // SETTINGSDIALOGTESTS_H
