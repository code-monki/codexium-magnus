#include "BibliographySettingsWidgetTests.h"
#include <QApplication>
#include <QComboBox>
#include "UI/BibliographySettingsWidget.h"
#include "../../../src/codexium-magnus-core/Models/BibliographyConfig.h"

using namespace CodexiumMagnus::UI;
using namespace CodexiumMagnus::Core::Models;

void BibliographySettingsWidgetTests::initTestCase() {
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
}

void BibliographySettingsWidgetTests::cleanupTestCase() {
    if (m_app != QApplication::instance()) {
        delete m_app;
    }
    m_app = nullptr;
}

void BibliographySettingsWidgetTests::init() {
    m_widget = new BibliographySettingsWidget();
}

void BibliographySettingsWidgetTests::cleanup() {
    delete static_cast<BibliographySettingsWidget*>(m_widget);
    m_widget = nullptr;
}

void BibliographySettingsWidgetTests::getConfig_DefaultValues_ReturnsDefaults() {
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    BibliographyConfig config = widget->getConfig();
    
    // Check default values (from widget setup)
    QCOMPARE(config.style, QString("APA"));
    QCOMPARE(config.sortBy, QString("author"));
    QCOMPARE(config.groupBy, QString("")); // None
}

void BibliographySettingsWidgetTests::setConfig_ValidConfig_UpdatesWidgets() {
    BibliographyConfig config;
    config.style = "CMS";
    config.sortBy = "year";
    config.groupBy = "year";
    
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.style, QString("CMS"));
    QCOMPARE(retrieved.sortBy, QString("year"));
    QCOMPARE(retrieved.groupBy, QString("year"));
}

void BibliographySettingsWidgetTests::getConfig_AfterSetConfig_ReturnsSameValues() {
    BibliographyConfig original;
    original.style = "APA";
    original.sortBy = "title";
    original.groupBy = "author";
    
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(original);
    BibliographyConfig retrieved = widget->getConfig();
    
    QCOMPARE(retrieved.style, original.style);
    QCOMPARE(retrieved.sortBy, original.sortBy);
    QCOMPARE(retrieved.groupBy, original.groupBy);
}

void BibliographySettingsWidgetTests::getConfig_StyleAPA_ReturnsAPA() {
    BibliographyConfig config;
    config.style = "APA";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.style, QString("APA"));
}

void BibliographySettingsWidgetTests::getConfig_StyleCMS_ReturnsCMS() {
    BibliographyConfig config;
    config.style = "CMS";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.style, QString("CMS"));
}

void BibliographySettingsWidgetTests::setConfig_StyleChange_UpdatesComboBox() {
    BibliographyConfig config;
    config.style = "CMS";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    
    // Verify widget reflects the change
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.style, QString("CMS"));
}

void BibliographySettingsWidgetTests::getConfig_SortByAuthor_ReturnsAuthor() {
    BibliographyConfig config;
    config.sortBy = "author";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.sortBy, QString("author"));
}

void BibliographySettingsWidgetTests::getConfig_SortByYear_ReturnsYear() {
    BibliographyConfig config;
    config.sortBy = "year";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.sortBy, QString("year"));
}

void BibliographySettingsWidgetTests::getConfig_SortByTitle_ReturnsTitle() {
    BibliographyConfig config;
    config.sortBy = "title";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.sortBy, QString("title"));
}

void BibliographySettingsWidgetTests::getConfig_GroupByNone_ReturnsEmpty() {
    BibliographyConfig config;
    config.groupBy = "";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.groupBy, QString(""));
}

void BibliographySettingsWidgetTests::getConfig_GroupByYear_ReturnsYear() {
    BibliographyConfig config;
    config.groupBy = "year";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.groupBy, QString("year"));
}

void BibliographySettingsWidgetTests::getConfig_GroupByAuthor_ReturnsAuthor() {
    BibliographyConfig config;
    config.groupBy = "author";
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(config);
    BibliographyConfig retrieved = widget->getConfig();
    QCOMPARE(retrieved.groupBy, QString("author"));
}

void BibliographySettingsWidgetTests::setConfig_CompleteConfig_AllFieldsPreserved() {
    BibliographyConfig original;
    original.style = "CMS";
    original.sortBy = "year";
    original.groupBy = "author";
    
    BibliographySettingsWidget* widget = static_cast<BibliographySettingsWidget*>(m_widget);
    widget->setConfig(original);
    BibliographyConfig retrieved = widget->getConfig();
    
    QCOMPARE(retrieved.style, original.style);
    QCOMPARE(retrieved.sortBy, original.sortBy);
    QCOMPARE(retrieved.groupBy, original.groupBy);
}

// QTEST_MAIN removed - using main.cpp instead
#include "BibliographySettingsWidgetTests.moc"
