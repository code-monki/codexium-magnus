#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include "../codexium-magnus-core/Models/TypographyConfig.h"
#include "../codexium-magnus-core/Models/BibliographyConfig.h"
#include "UI/NavigationPane.h"
#include "UI/SearchPane.h"
#include "UI/SettingsDialog.h"
#include "Services/CartridgeService.h"
#include "Services/SearchService.h"
#include "Services/LinkService.h"
#include "Services/PrintService.h"

// Placeholder configuration sources
namespace CodexiumMagnus {

// Zoom constants
const qreal MainWindow::ZOOM_MIN = 0.25;
const qreal MainWindow::ZOOM_MAX = 5.0;
const qreal MainWindow::ZOOM_STEP = 0.1;
const qreal MainWindow::ZOOM_DEFAULT = 1.0;

class MainWindow::SystemConfigSource : public Core::Configuration::ConfigurationSource {
public:
    Core::Models::TypographyConfig* getTypography() override {
        static Core::Models::TypographyConfig config;
        config.baseFontFamily = "Segoe UI";
        config.baseFontSizePt = 12.0;
        return &config;
    }
    
    Core::Models::BibliographyConfig* getBibliography() override {
        static Core::Models::BibliographyConfig config;
        config.style = "APA";
        return &config;
    }
};

class MainWindow::CorpusConfigSource : public Core::Configuration::ConfigurationSource {
public:
    Core::Models::TypographyConfig* getTypography() override { return nullptr; }
    Core::Models::BibliographyConfig* getBibliography() override { return nullptr; }
};

class MainWindow::UserProfileConfigSource : public Core::Configuration::ConfigurationSource {
public:
    Core::Models::TypographyConfig* getTypography() override {
        static Core::Models::TypographyConfig config;
        QSettings settings("CodexiumMagnus", "Settings");
        
        config.baseFontFamily = settings.value("typography/fontFamily", "system-ui").toString();
        config.baseFontSizePt = settings.value("typography/fontSize", 12.0).toDouble();
        
        // Load heading scales
        QList<QVariant> scales = settings.value("typography/headingScales").toList();
        if (scales.isEmpty()) {
            config.headingScale = {2.0, 1.75, 1.5, 1.25, 1.1, 1.0};
        } else {
            config.headingScale.clear();
            for (const QVariant& scale : scales) {
                config.headingScale.append(scale.toDouble());
            }
        }
        
        config.printOptions.pageMarginMm = settings.value("typography/printMargin", 10.0).toDouble();
        config.printOptions.blackOnWhite = settings.value("typography/blackOnWhite", false).toBool();
        
        return &config;
    }

    Core::Models::BibliographyConfig* getBibliography() override {
        static Core::Models::BibliographyConfig config;
        QSettings settings("CodexiumMagnus", "Settings");
        
        config.style = settings.value("bibliography/style", "APA").toString();
        config.sortBy = settings.value("bibliography/sortBy", "author").toString();
        config.groupBy = settings.value("bibliography/groupBy", "").toString();
        
        return &config;
    }
};

class MainWindow::SessionConfigSource : public Core::Configuration::ConfigurationSource {
public:
    SessionConfigSource() : m_typography(nullptr), m_bibliography(nullptr) {}
    
    Core::Models::TypographyConfig* getTypography() override {
        return m_typography;
    }
    
    Core::Models::BibliographyConfig* getBibliography() override {
        return m_bibliography;
    }
    
    void setTypography(Core::Models::TypographyConfig* typ) { m_typography = typ; }
    void setBibliography(Core::Models::BibliographyConfig* bib) { m_bibliography = bib; }
    
private:
    Core::Models::TypographyConfig* m_typography;
    Core::Models::BibliographyConfig* m_bibliography;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainSplitter(nullptr)
    , m_leftSplitter(nullptr)
    , m_webEngineView(nullptr)
    , m_navigationPane(nullptr)
    , m_searchPane(nullptr)
    , m_configResolver(nullptr)
    , m_webEngineBridge(nullptr)
    , m_cartridgeService(nullptr)
    , m_signatureService(nullptr)
    , m_sessionConfigSource(nullptr)
    , m_searchService(nullptr)
    , m_linkService(nullptr)
    , m_printService(nullptr)
    , m_themeManager(nullptr)
    , m_themeActionGroup(nullptr)
    , m_themeLightAction(nullptr)
    , m_themeSepiaAction(nullptr)
    , m_themeDarkAction(nullptr)
    , m_themeCustomAction(nullptr)
    , m_zoomFactor(ZOOM_DEFAULT)
{
    // Load saved zoom factor from settings
    QSettings settings("CodexiumMagnus", "Settings");
    m_zoomFactor = settings.value("view/zoomFactor", ZOOM_DEFAULT).toDouble();
    
    // Clamp to valid range
    if (m_zoomFactor < ZOOM_MIN) m_zoomFactor = ZOOM_MIN;
    if (m_zoomFactor > ZOOM_MAX) m_zoomFactor = ZOOM_MAX;
    
    // Initialize ThemeManager first (applies stylesheet to QApplication)
    m_themeManager = new Theme::ThemeManager(this);
    connect(m_themeManager, &Theme::ThemeManager::themeChanged,
            this, &MainWindow::onThemeChanged);
    
    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupWebEngine();
    setupThemeMenu();
    
    // Setup configuration sources (order matters: Session → User → Corpus → System)
    SessionConfigSource* sessionSource = new SessionConfigSource();
    m_configSources.append(sessionSource);
    m_configSources.append(new UserProfileConfigSource());
    m_configSources.append(new CorpusConfigSource());
    m_configSources.append(new SystemConfigSource());
    
    // Store session source for runtime updates
    m_sessionConfigSource = sessionSource;
    
    m_configResolver = new Core::Configuration::CompositeConfigurationResolver(m_configSources);
    
    // Create services
    // Create signature service first
    m_signatureService = new Services::SignatureService(this);
    
    // Create cartridge service and connect signature service
    m_cartridgeService = new Services::CartridgeService(this);
    static_cast<Services::CartridgeService*>(m_cartridgeService)->setSignatureService(m_signatureService);
    
    m_searchService = new Services::SearchService(m_cartridgeService, this);
    m_linkService = new Services::LinkService(this);
    m_printService = new Services::PrintService(m_webEngineView, this);
    
    // Connect signals
    connect(m_cartridgeService, &Services::ICartridgeService::cartridgeLoaded,
            this, &MainWindow::onCartridgeLoaded);
    connect(m_cartridgeService, &Services::ICartridgeService::cartridgeUnloaded,
            this, &MainWindow::onCartridgeUnloaded);
    connect(static_cast<Services::CartridgeService*>(m_cartridgeService), 
            &Services::CartridgeService::trustLevelDetermined,
            this, &MainWindow::onTrustLevelDetermined);
    connect(m_searchPane, &UI::SearchPane::searchRequested,
            this, &MainWindow::onSearchRequested);
    connect(m_searchService, &Services::ISearchService::searchCompleted,
            this, &MainWindow::onSearchCompleted);
    connect(m_navigationPane, &UI::NavigationPane::documentSelected,
            this, &MainWindow::onDocumentSelected);
    connect(m_searchPane, &UI::SearchPane::resultSelected,
            this, &MainWindow::onResultSelected);
    connect(m_linkService, &Services::ILinkService::linkOpened,
            this, [this](const QUrl& url) {
                statusBar()->showMessage(QString("Opened link: %1").arg(url.toString()), 2000);
            });
    connect(m_printService, &Services::IPrintService::printCompleted,
            this, [this]() {
                statusBar()->showMessage("Print completed", 2000);
            });
    connect(m_printService, &Services::IPrintService::printError,
            this, [this](const QString& error) {
                QMessageBox::warning(this, "Print Error", error);
            });
    
    // Connect WebEngine load finished signal
    connect(m_webEngineView, &QWebEngineView::loadFinished, 
            this, &MainWindow::onWebEngineLoadFinished);
    
    // Connect WebEngine link clicks
    connect(m_webEngineView, &QWebEngineView::urlChanged,
            this, [this](const QUrl& url) {
                // Intercept external links
                if (url.scheme() == "http" || url.scheme() == "https" || 
                    url.scheme() == "mailto" || url.scheme() == "ftp") {
                    m_linkService->openExternalLink(url);
                    // Prevent navigation in WebEngine
                    m_webEngineView->back();
                }
            });
}

MainWindow::~MainWindow() {
    qDeleteAll(m_configSources);
    delete m_configResolver;
}

void MainWindow::setupUi() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Left splitter for navigation and search
    m_leftSplitter = new QSplitter(Qt::Vertical, m_centralWidget);
    m_navigationPane = new UI::NavigationPane(m_leftSplitter);
    m_searchPane = new UI::SearchPane(m_leftSplitter);
    m_leftSplitter->addWidget(m_navigationPane);
    m_leftSplitter->addWidget(m_searchPane);
    m_leftSplitter->setStretchFactor(0, 2);
    m_leftSplitter->setStretchFactor(1, 1);
    
    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, m_centralWidget);
    m_mainSplitter->addWidget(m_leftSplitter);
    // WebEngine view will be added in setupWebEngine()
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setSizes({200, 600});
    
    mainLayout->addWidget(m_mainSplitter);
}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open Cartridge...", this, &MainWindow::onOpenCartridge, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("&Print...", this, &MainWindow::onPrint, QKeySequence::Print);
    fileMenu->addAction("Print to &PDF...", this, &MainWindow::onPrintToPdf);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
    
    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Zoom In", this, &MainWindow::onZoomIn, QKeySequence::ZoomIn);
    viewMenu->addAction("Zoom &Out", this, &MainWindow::onZoomOut, QKeySequence::ZoomOut);
    viewMenu->addAction("&Normal Size", this, &MainWindow::onZoomReset, QKeySequence("Ctrl+0"));
    
    QMenu *themeMenu = menuBar()->addMenu("&Theme");
    setupThemeMenu();
    
    QMenu *settingsMenu = menuBar()->addMenu("&Settings");
    settingsMenu->addAction("&Preferences...", this, &MainWindow::onSettings, QKeySequence::Preferences);
    
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *helpAction = helpMenu->addAction("&Help...", this, &MainWindow::onHelp);
    helpAction->setShortcut(QKeySequence(QKeySequence::HelpContents));
    helpMenu->addSeparator();
    helpMenu->addAction("&About", this, []() {
        QMessageBox::about(nullptr, "About Codexium Magnus",
            "Codexium Magnus v1.0.0\n\n"
            "A cross-platform archival reader built with Qt 6 and WebEngine.\n\n"
            "Copyright © 2025");
    });
}

void MainWindow::setupThemeMenu() {
    QMenu *themeMenu = menuBar()->findChild<QMenu*>("&Theme");
    if (!themeMenu) {
        themeMenu = menuBar()->addMenu("&Theme");
    }
    
    m_themeActionGroup = new QActionGroup(this);
    
    m_themeLightAction = new QAction("&Light", this);
    m_themeLightAction->setCheckable(true);
    m_themeLightAction->setActionGroup(m_themeActionGroup);
    connect(m_themeLightAction, &QAction::triggered, this, &MainWindow::onThemeLight);
    
    m_themeSepiaAction = new QAction("&Sepia", this);
    m_themeSepiaAction->setCheckable(true);
    m_themeSepiaAction->setActionGroup(m_themeActionGroup);
    connect(m_themeSepiaAction, &QAction::triggered, this, &MainWindow::onThemeSepia);
    
    m_themeDarkAction = new QAction("&Dark", this);
    m_themeDarkAction->setCheckable(true);
    m_themeDarkAction->setActionGroup(m_themeActionGroup);
    connect(m_themeDarkAction, &QAction::triggered, this, &MainWindow::onThemeDark);
    
    m_themeCustomAction = new QAction("&Custom...", this);
    m_themeCustomAction->setCheckable(true);
    m_themeCustomAction->setActionGroup(m_themeActionGroup);
    connect(m_themeCustomAction, &QAction::triggered, this, &MainWindow::onThemeCustom);
    
    themeMenu->addAction(m_themeLightAction);
    themeMenu->addAction(m_themeSepiaAction);
    themeMenu->addAction(m_themeDarkAction);
    themeMenu->addSeparator();
    themeMenu->addAction(m_themeCustomAction);
    
    // Set current theme action as checked
    onThemeChanged(m_themeManager->currentTheme());
}

void MainWindow::setupToolBar() {
    QToolBar *toolBar = addToolBar("Main");
    toolBar->setMovable(false);
    
    toolBar->addAction("Open", this, &MainWindow::onOpenCartridge);
    toolBar->addSeparator();
    toolBar->addAction("Search", this, [this]() {
        m_searchPane->setFocus();
    });
    toolBar->addSeparator();
    toolBar->addAction("Print", this, &MainWindow::onPrint);
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage("Ready");
}

void MainWindow::setupWebEngine() {
    m_webEngineView = new QWebEngineView(m_mainSplitter);
    m_mainSplitter->addWidget(m_webEngineView);
    
    // Set initial zoom factor
    m_webEngineView->setZoomFactor(m_zoomFactor);
    
    // Create WebEngine bridge
    m_webEngineBridge = new Services::WebEngineBridge(m_webEngineView, this);
    
    // Load minimal empty state (WCAG-compliant: clean main content area)
    QString htmlContent = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <title>Codexium Magnus</title>
            <style>
                body {
                    font-family: system-ui, -apple-system, 'Segoe UI', sans-serif;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    min-height: 100vh;
                    margin: 0;
                    padding: 40px;
                    text-align: center;
                }
                .empty-state {
                    max-width: 500px;
                }
                h1 {
                    font-size: 1.8em;
                    margin-bottom: 0.5em;
                    font-weight: 300;
                }
                p {
                    font-size: 1em;
                    color: rgba(128, 128, 128, 0.8);
                    margin-top: 1em;
                }
            </style>
        </head>
        <body>
            <div class="empty-state">
                <h1>Codexium Magnus</h1>
                <p>Open a cartridge to begin reading.</p>
                <p>Go to <strong>File → Open Cartridge...</strong> or press <strong>Ctrl+O</strong> / <strong>Cmd+O</strong></p>
            </div>
        </body>
        </html>
    )";
    
    m_webEngineView->setHtml(wrapContentWithTheme(htmlContent));
}

void MainWindow::onWebEngineLoadFinished(bool success) {
    if (success) {
        pushConfigToWebEngine();
        injectThemeTokens(m_currentDocumentContent);
        statusBar()->showMessage("Page loaded", 2000);
    } else {
        statusBar()->showMessage("Failed to load page", 2000);
    }
}

void MainWindow::pushConfigToWebEngine() {
    if (!m_configResolver || !m_webEngineBridge) {
        return;
    }
    
    auto typography = m_configResolver->getEffectiveTypography();
    auto bibliography = m_configResolver->getEffectiveBibliography();
    
    QJsonObject payload;
    payload["type"] = "config:update";
    
    QJsonObject typographyObj;
    typographyObj["baseFontFamily"] = typography.baseFontFamily;
    typographyObj["baseFontSizePt"] = typography.baseFontSizePt;
    
    QJsonArray headingScaleArray;
    for (double scale : typography.headingScale) {
        headingScaleArray.append(scale);
    }
    typographyObj["headingScale"] = headingScaleArray;
    
    QJsonObject printOptionsObj;
    printOptionsObj["pageMarginMm"] = typography.printOptions.pageMarginMm;
    printOptionsObj["blackOnWhite"] = typography.printOptions.blackOnWhite;
    typographyObj["printOptions"] = printOptionsObj;
    
    payload["typography"] = typographyObj;
    
    QJsonObject bibliographyObj;
    bibliographyObj["style"] = bibliography.style;
    bibliographyObj["sortBy"] = bibliography.sortBy;
    bibliographyObj["groupBy"] = bibliography.groupBy;
    payload["bibliography"] = bibliographyObj;
    
    QJsonDocument doc(payload);
    QString jsonString = QString::fromUtf8(doc.toJson());
    
    m_webEngineBridge->send(jsonString);
}

void MainWindow::onOpenCartridge() {
    QString path = QFileDialog::getOpenFileName(this,
        "Open Cartridge",
        QDir::homePath(),
        "Cartridge Files (*.ruleset *.db *.sqlite *.sqlite3);;All Files (*.*)");
    
    if (!path.isEmpty()) {
        if (!m_cartridgeService->loadCartridge(path)) {
            QMessageBox::warning(this, "Error", "Failed to load cartridge.");
        }
    }
}

void MainWindow::onCartridgeLoaded(const QString& cartridgeName) {
    statusBar()->showMessage(QString("Cartridge loaded: %1").arg(cartridgeName), 3000);
    m_navigationPane->setNavigationModel(m_cartridgeService->getNavigationModel());
    setWindowTitle(QString("Codexium Magnus - %1").arg(cartridgeName));
}

void MainWindow::onCartridgeUnloaded() {
    statusBar()->showMessage("Cartridge unloaded", 2000);
    m_navigationPane->clear();
    setWindowTitle("Codexium Magnus");
}

void MainWindow::onSearchRequested(const QString& query) {
    m_searchService->performSearch(query,
                                  m_searchPane->isCaseSensitive(),
                                  m_searchPane->isFuzzy(),
                                  m_searchPane->useWildcards());
}

void MainWindow::onSearchCompleted(const QList<QPair<QString, QString>>& results) {
    m_searchPane->setResults(results);
}

void MainWindow::onDocumentSelected(const QString& documentId) {
    loadDocument(documentId);
}

void MainWindow::onResultSelected(const QString& resultId) {
    loadDocument(resultId);
}

void MainWindow::loadDocument(const QString& documentId) {
    if (!m_cartridgeService->isCartridgeLoaded()) {
        return;
    }
    
    QString content = m_cartridgeService->getDocumentContent(documentId);
    if (!content.isEmpty()) {
        m_currentDocumentContent = content;
        QString wrappedContent = wrapContentWithTheme(content);
        m_webEngineView->setHtml(wrappedContent);
    }
}

void MainWindow::onThemeChanged(Theme::ThemeManager::Theme theme) {
    // Update theme menu check state
    switch (theme) {
        case Theme::ThemeManager::Light:
            m_themeLightAction->setChecked(true);
            break;
        case Theme::ThemeManager::Sepia:
            m_themeSepiaAction->setChecked(true);
            break;
        case Theme::ThemeManager::Dark:
            m_themeDarkAction->setChecked(true);
            break;
        case Theme::ThemeManager::Custom:
            m_themeCustomAction->setChecked(true);
            break;
    }
    
    // Re-inject theme tokens into current document
    if (!m_currentDocumentContent.isEmpty()) {
        QString wrappedContent = wrapContentWithTheme(m_currentDocumentContent);
        m_webEngineView->setHtml(wrappedContent);
    }
    
    // Update application palette
    QApplication::setPalette(m_themeManager->currentPalette());
}

void MainWindow::onThemeLight() {
    m_themeManager->setTheme(Theme::ThemeManager::Light);
}

void MainWindow::onThemeSepia() {
    m_themeManager->setTheme(Theme::ThemeManager::Sepia);
}

void MainWindow::onThemeDark() {
    m_themeManager->setTheme(Theme::ThemeManager::Dark);
}

void MainWindow::onThemeCustom() {
    QString path = QFileDialog::getOpenFileName(this,
        "Load Custom Theme",
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),
        "Theme Files (*.qss);;All Files (*.*)");
    
    if (!path.isEmpty()) {
        m_themeManager->loadCustomTheme(path);
    } else {
        // Reset to previous theme if cancelled
        onThemeChanged(m_themeManager->currentTheme());
    }
}

MainWindow::SessionConfigSource* MainWindow::getSessionConfigSource() const {
    return m_sessionConfigSource;
}

void MainWindow::onSettings() {
    UI::SettingsDialog dialog(this);
    
    // Load current settings
    auto typography = m_configResolver->getEffectiveTypography();
    auto bibliography = m_configResolver->getEffectiveBibliography();
    dialog.setTypographyConfig(typography);
    dialog.setBibliographyConfig(bibliography);
    
    // Connect to settings accepted signal
    connect(&dialog, &UI::SettingsDialog::settingsAccepted,
            this, &MainWindow::onSettingsAccepted);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Settings are saved in the dialog, and onSettingsAccepted is called
    }
}

void MainWindow::onHelp() {
    UI::HelpDialog dialog(this);
    dialog.exec();
}

void MainWindow::onSettingsAccepted(const Core::Models::TypographyConfig& typography,
                                    const Core::Models::BibliographyConfig& bibliography) {
    // Update session config source with new settings
    if (m_sessionConfigSource) {
        // Create new config objects for session override
        static Core::Models::TypographyConfig sessionTypography;
        static Core::Models::BibliographyConfig sessionBibliography;
        
        sessionTypography = typography;
        sessionBibliography = bibliography;
        
        m_sessionConfigSource->setTypography(&sessionTypography);
        m_sessionConfigSource->setBibliography(&sessionBibliography);
    }
    
    // Push updated config to WebEngine
    pushConfigToWebEngine();
    
    // Re-inject theme tokens if document is loaded
    if (!m_currentDocumentContent.isEmpty()) {
        QString wrappedContent = wrapContentWithTheme(m_currentDocumentContent);
        m_webEngineView->setHtml(wrappedContent);
    }
    
    statusBar()->showMessage("Settings saved and applied", 2000);
}

void MainWindow::onTrustLevelDetermined(Services::TrustLevel trustLevel) {
    QString trustMessage;
    QString trustIcon;
    
    switch (trustLevel) {
        case Services::TrustLevel::Official:
            trustMessage = "Official cartridge - verified signature";
            trustIcon = "✓";
            break;
        case Services::TrustLevel::Verified:
            trustMessage = "Verified cartridge - trusted publisher";
            trustIcon = "✓";
            break;
        case Services::TrustLevel::Unverified:
            trustMessage = "Unverified cartridge - signature not in trusted store";
            trustIcon = "?";
            break;
        case Services::TrustLevel::Homebrew:
            trustMessage = "Unsigned cartridge - no signature";
            trustIcon = "⚠";
            break;
        case Services::TrustLevel::Invalid:
            trustMessage = "Invalid cartridge - signature verification failed";
            trustIcon = "✗";
            QMessageBox::warning(this, "Cartridge Verification", 
                "This cartridge's signature could not be verified. "
                "It may have been tampered with or corrupted.");
            break;
    }
    
    statusBar()->showMessage(QString("%1 %2").arg(trustIcon, trustMessage), 5000);
}

void MainWindow::onPrint() {
    if (m_currentDocumentContent.isEmpty()) {
        QMessageBox::information(this, "Print", "No document loaded to print.");
        return;
    }
    
    m_printService->printContent(m_currentDocumentContent);
}

void MainWindow::onPrintToPdf() {
    if (m_currentDocumentContent.isEmpty()) {
        QMessageBox::information(this, "Print to PDF", "No document loaded to print.");
        return;
    }
    
    QString path = QFileDialog::getSaveFileName(this,
        "Save PDF",
        QDir::homePath(),
        "PDF Files (*.pdf);;All Files (*.*)");
    
    if (!path.isEmpty()) {
        m_printService->printToPdf(m_currentDocumentContent, path);
    }
}

void MainWindow::onLinkClicked(const QUrl& url) {
    m_linkService->openExternalLink(url);
}

QString MainWindow::wrapContentWithTheme(const QString& htmlContent) {
    // Get theme tokens
    QMap<QString, QString> tokens = m_themeManager->getTokenMap();
    
    // Build CSS with theme tokens
    QString css = "<style>\n";
    css += ":root {\n";
    for (auto it = tokens.constBegin(); it != tokens.constEnd(); ++it) {
        css += QString("  %1: %2;\n").arg(it.key(), it.value());
    }
    css += "}\n";
    css += "body {\n";
    css += "  background-color: var(--color-background-primary);\n";
    css += "  color: var(--color-text-primary);\n";
    css += "  font-family: var(--font-family-base);\n";
    css += "  font-size: var(--font-size-base);\n";
    css += "}\n";
    css += "a {\n";
    css += "  color: var(--color-link);\n";
    css += "}\n";
    css += "a:hover {\n";
    css += "  color: var(--color-link-hover);\n";
    css += "}\n";
    css += "</style>\n";
    
    // Inject CSS into HTML
    QString wrapped = htmlContent;
    
    // Find </head> or <body> tag and insert CSS before it
    int headEnd = wrapped.indexOf("</head>", 0, Qt::CaseInsensitive);
    if (headEnd != -1) {
        wrapped.insert(headEnd, css);
    } else {
        // No </head> tag, insert before <body> or at start
        int bodyStart = wrapped.indexOf("<body", 0, Qt::CaseInsensitive);
        if (bodyStart != -1) {
            wrapped.insert(bodyStart, css);
        } else {
            // No body tag either, prepend
            wrapped = css + wrapped;
        }
    }
    
    return wrapped;
}

void MainWindow::injectThemeTokens(const QString& htmlContent) {
    // This method can be used to inject tokens via JavaScript if needed
    // For now, tokens are injected via CSS in wrapContentWithTheme()
    Q_UNUSED(htmlContent);
}

void MainWindow::onZoomIn() {
    qreal newZoom = m_zoomFactor + ZOOM_STEP;
    if (newZoom > ZOOM_MAX) {
        newZoom = ZOOM_MAX;
    }
    setZoomFactor(newZoom);
}

void MainWindow::onZoomOut() {
    qreal newZoom = m_zoomFactor - ZOOM_STEP;
    if (newZoom < ZOOM_MIN) {
        newZoom = ZOOM_MIN;
    }
    setZoomFactor(newZoom);
}

void MainWindow::onZoomReset() {
    setZoomFactor(ZOOM_DEFAULT);
}

void MainWindow::setZoomFactor(qreal factor) {
    // Clamp to valid range
    if (factor < ZOOM_MIN) factor = ZOOM_MIN;
    if (factor > ZOOM_MAX) factor = ZOOM_MAX;
    
    m_zoomFactor = factor;
    
    // Apply zoom to WebEngine view
    if (m_webEngineView) {
        m_webEngineView->setZoomFactor(m_zoomFactor);
    }
    
    // Save zoom factor to settings
    QSettings settings("CodexiumMagnus", "Settings");
    settings.setValue("view/zoomFactor", m_zoomFactor);
    
    // Update zoom display in status bar
    updateZoomDisplay();
}

void MainWindow::updateZoomDisplay() {
    if (statusBar()) {
        // Format zoom as percentage (e.g., "125%")
        int zoomPercent = static_cast<int>(m_zoomFactor * 100);
        statusBar()->showMessage(QString("Zoom: %1%").arg(zoomPercent), 2000);
    }
}

} // namespace CodexiumMagnus