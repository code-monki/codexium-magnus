#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QFileDialog>
#include <QActionGroup>
#include "../codexium-magnus-core/Configuration/CompositeConfigurationResolver.h"
#include "Services/WebEngineBridge.h"
#include "Services/ICartridgeService.h"
#include "Services/CartridgeService.h"
#include "Services/ISearchService.h"
#include "Services/SearchService.h"
#include "Services/ILinkService.h"
#include "Services/LinkService.h"
#include "Services/IPrintService.h"
#include "Services/PrintService.h"
#include "Services/ISignatureService.h"
#include "Services/SignatureService.h"
#include "Theme/ThemeManager.h"
#include "UI/NavigationPane.h"
#include "UI/SearchPane.h"
#include "UI/SettingsDialog.h"
#include "UI/HelpDialog.h"

namespace CodexiumMagnus {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onWebEngineLoadFinished(bool success);
    void pushConfigToWebEngine();
    void onOpenCartridge();
    void onCartridgeLoaded(const QString& cartridgeName);
    void onCartridgeUnloaded();
    void onSearchRequested(const QString& query);
    void onSearchCompleted(const QList<QPair<QString, QString>>& results);
    void onDocumentSelected(const QString& documentId);
    void onResultSelected(const QString& resultId);
    void onThemeChanged(Theme::ThemeManager::Theme theme);
    void onThemeLight();
    void onThemeSepia();
    void onThemeDark();
    void onThemeCustom();
    void onPrint();
    void onPrintToPdf();
    void onLinkClicked(const QUrl& url);
    void onTrustLevelDetermined(Services::TrustLevel trustLevel);
    void onSettings();
    void onSettingsAccepted(const Core::Models::TypographyConfig& typography,
                           const Core::Models::BibliographyConfig& bibliography);
    void onHelp();
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void setZoomFactor(qreal factor);
    void updateZoomDisplay();

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupWebEngine();
    void setupThemeMenu();
    void loadDocument(const QString& documentId);
    void injectThemeTokens(const QString& htmlContent);
    QString wrapContentWithTheme(const QString& htmlContent);

    // UI Components
    QWidget *m_centralWidget;
    QSplitter *m_mainSplitter;
    QSplitter *m_leftSplitter;
    QWebEngineView *m_webEngineView;
    UI::NavigationPane *m_navigationPane;
    UI::SearchPane *m_searchPane;
    
    // Services
    Core::Configuration::CompositeConfigurationResolver *m_configResolver;
    Services::WebEngineBridge *m_webEngineBridge;
    Services::ICartridgeService *m_cartridgeService;
    Services::ISearchService *m_searchService;
    Services::ILinkService *m_linkService;
    Services::IPrintService *m_printService;
    Services::ISignatureService *m_signatureService;
    Theme::ThemeManager *m_themeManager;
    
    // Theme menu
    QActionGroup *m_themeActionGroup;
    QAction *m_themeLightAction;
    QAction *m_themeSepiaAction;
    QAction *m_themeDarkAction;
    QAction *m_themeCustomAction;
    
    // Configuration sources (placeholder implementations)
    class SystemConfigSource;
    class CorpusConfigSource;
    class UserProfileConfigSource;
    class SessionConfigSource;
    
    SessionConfigSource* getSessionConfigSource() const;
    
    QList<Core::Configuration::ConfigurationSource*> m_configSources;
    SessionConfigSource* m_sessionConfigSource;  ///< Session config source for runtime updates
    
    // Current document content (for printing)
    QString m_currentDocumentContent;
    
    // Zoom state
    qreal m_zoomFactor;
    static const qreal ZOOM_MIN;
    static const qreal ZOOM_MAX;
    static const qreal ZOOM_STEP;
    static const qreal ZOOM_DEFAULT;
};

} // namespace CodexiumMagnus

#endif // MAINWINDOW_H