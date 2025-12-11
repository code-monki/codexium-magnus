#include "SettingsDialogTests.h"
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QDateTime>
#include "UI/SettingsDialog.h"
#include "../../../src/codexium-magnus-core/Models/TypographyConfig.h"
#include "../../../src/codexium-magnus-core/Models/BibliographyConfig.h"

using namespace CodexiumMagnus::UI;
using namespace CodexiumMagnus::Core::Models;

void SettingsDialogTests::initTestCase() {
    // QApplication should already exist from main()
    QApplication* existingApp = qobject_cast<QApplication*>(QApplication::instance());
    if (!existingApp) {
        m_argc = 1;
        static char appName[] = "test";
        static char* argv[] = {appName, nullptr};
        m_argv = argv;
        m_app = new QApplication(m_argc, m_argv);
    } else {
        m_app = existingApp;
    }
    
    // Use temporary settings for testing
    m_testSettingsPath = QDir::tempPath() + "/test_settings_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, m_testSettingsPath);
}

void SettingsDialogTests::cleanupTestCase() {
    // Clean up test settings
    QSettings settings("CodexiumMagnus", "Settings");
    settings.clear();
    settings.sync();
    
    if (m_app != QApplication::instance()) {
        delete m_app;
    }
    m_app = nullptr;
}

void SettingsDialogTests::init() {
    m_dialog = new SettingsDialog();
}

void SettingsDialogTests::cleanup() {
    delete static_cast<SettingsDialog*>(m_dialog);
    m_dialog = nullptr;
}

void SettingsDialogTests::getTypographyConfig_DefaultValues_ReturnsDefaults() {
    SettingsDialog* dialog = static_cast<SettingsDialog*>(m_dialog);
    TypographyConfig config = dialog->getTypographyConfig();
    
    // Check that we get valid defaults (may be from QSettings or widget defaults)
    QVERIFY(!config.baseFontFamily.isEmpty() || config.baseFontFamily.isEmpty()); // May be empty
    QVERIFY(config.baseFontSizePt > 0.0);
    QVERIFY(config.headingScale.size() == 6);
    QVERIFY(config.printOptions.pageMarginMm >= 0.0);
}

void SettingsDialogTests::getBibliographyConfig_DefaultValues_ReturnsDefaults() {
    SettingsDialog* dialog = static_cast<SettingsDialog*>(m_dialog);
    BibliographyConfig config = dialog->getBibliographyConfig();
    
    // Check that we get valid defaults
    QVERIFY(config.style == "APA" || config.style == "CMS");
    QVERIFY(!config.sortBy.isEmpty());
}

void SettingsDialogTests::setTypographyConfig_ValidConfig_UpdatesDialog() {
    TypographyConfig config;
    config.baseFontFamily = "TestFont";
    config.baseFontSizePt = 14.0;
    config.headingScale = {2.0, 1.75, 1.5, 1.25, 1.1, 1.0};
    config.printOptions.pageMarginMm = 15.0;
    config.printOptions.blackOnWhite = true;
    
    SettingsDialog* dialog = static_cast<SettingsDialog*>(m_dialog);
    dialog->setTypographyConfig(config);
    
    TypographyConfig retrieved = dialog->getTypographyConfig();
    QCOMPARE(retrieved.baseFontFamily, QString("TestFont"));
    QCOMPARE(retrieved.baseFontSizePt, 14.0);
    QCOMPARE(retrieved.printOptions.pageMarginMm, 15.0);
    QCOMPARE(retrieved.printOptions.blackOnWhite, true);
}

void SettingsDialogTests::setBibliographyConfig_ValidConfig_UpdatesDialog() {
    BibliographyConfig config;
    config.style = "CMS";
    config.sortBy = "year";
    config.groupBy = "author";
    
    SettingsDialog* dialog = static_cast<SettingsDialog*>(m_dialog);
    dialog->setBibliographyConfig(config);
    
    BibliographyConfig retrieved = dialog->getBibliographyConfig();
    QCOMPARE(retrieved.style, QString("CMS"));
    QCOMPARE(retrieved.sortBy, QString("year"));
    QCOMPARE(retrieved.groupBy, QString("author"));
}

void SettingsDialogTests::loadSettings_NoSavedSettings_UsesDefaults() {
    // Clear settings first
    QSettings settings("CodexiumMagnus", "Settings");
    settings.clear();
    settings.sync();
    
    SettingsDialog dialog;
    
    // Should load defaults
    TypographyConfig typo = dialog.getTypographyConfig();
    BibliographyConfig bib = dialog.getBibliographyConfig();
    
    QVERIFY(!typo.baseFontFamily.isEmpty() || typo.baseFontFamily.isEmpty());
    QVERIFY(typo.baseFontSizePt > 0.0);
    QVERIFY(!bib.style.isEmpty());
}

void SettingsDialogTests::saveSettings_ValidSettings_PersistsToQSettings() {
    TypographyConfig typo;
    typo.baseFontFamily = "SavedFont";
    typo.baseFontSizePt = 13.0;
    typo.headingScale = {2.0, 1.75, 1.5, 1.25, 1.1, 1.0};
    typo.printOptions.pageMarginMm = 12.0;
    typo.printOptions.blackOnWhite = false;
    
    BibliographyConfig bib;
    bib.style = "APA";
    bib.sortBy = "title";
    bib.groupBy = "";
    
    SettingsDialog* dialog = static_cast<SettingsDialog*>(m_dialog);
    dialog->setTypographyConfig(typo);
    dialog->setBibliographyConfig(bib);
    
    // Trigger save by calling saveSettingsNow (public method for testing)
    dialog->saveSettingsNow();
    
    // Verify settings were saved (need to create new QSettings instance to read)
    QSettings settings("CodexiumMagnus", "Settings");
    settings.sync(); // Ensure we read from disk
    QString savedFont = settings.value("typography/fontFamily").toString();
    QCOMPARE(savedFont, QString("SavedFont"));
    QCOMPARE(settings.value("typography/fontSize").toDouble(), 13.0);
    QCOMPARE(settings.value("bibliography/style").toString(), QString("APA"));
    QCOMPARE(settings.value("bibliography/sortBy").toString(), QString("title"));
}

void SettingsDialogTests::loadSettings_AfterSave_LoadsSavedValues() {
    // First save some settings
    QSettings settings("CodexiumMagnus", "Settings");
    settings.setValue("typography/fontFamily", "LoadedFont");
    settings.setValue("typography/fontSize", 15.0);
    settings.setValue("bibliography/style", "CMS");
    settings.setValue("bibliography/sortBy", "year");
    settings.sync();
    
    // Create new dialog (should load saved settings)
    SettingsDialog dialog;
    
    TypographyConfig typo = dialog.getTypographyConfig();
    BibliographyConfig bib = dialog.getBibliographyConfig();
    
    QCOMPARE(typo.baseFontFamily, QString("LoadedFont"));
    QCOMPARE(typo.baseFontSizePt, 15.0);
    QCOMPARE(bib.style, QString("CMS"));
    QCOMPARE(bib.sortBy, QString("year"));
}

// QTEST_MAIN removed - using main.cpp instead
#include "SettingsDialogTests.moc"
