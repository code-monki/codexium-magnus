#ifndef BIBLIOGRAPHYSETTINGSWIDGETTESTS_H
#define BIBLIOGRAPHYSETTINGSWIDGETTESTS_H

#include <QtTest/QtTest>

class BibliographySettingsWidgetTests : public QObject {
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
    
    // Style tests
    void getConfig_StyleAPA_ReturnsAPA();
    void getConfig_StyleCMS_ReturnsCMS();
    void setConfig_StyleChange_UpdatesComboBox();
    
    // Sort by tests
    void getConfig_SortByAuthor_ReturnsAuthor();
    void getConfig_SortByYear_ReturnsYear();
    void getConfig_SortByTitle_ReturnsTitle();
    
    // Group by tests
    void getConfig_GroupByNone_ReturnsEmpty();
    void getConfig_GroupByYear_ReturnsYear();
    void getConfig_GroupByAuthor_ReturnsAuthor();
    
    // Integration tests
    void setConfig_CompleteConfig_AllFieldsPreserved();

private:
    void* m_widget; // BibliographySettingsWidget* - using void* to avoid include in header
    int m_argc;
    char** m_argv;
    QApplication* m_app;
};

#endif // BIBLIOGRAPHYSETTINGSWIDGETTESTS_H
