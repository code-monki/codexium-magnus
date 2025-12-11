#include "DocumentViewerWindow.h"
#include <QApplication>
#include <QKeySequence>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

namespace CodexiumMagnus::UI {

// Zoom constants
const qreal DocumentViewerWindow::ZOOM_MIN = 0.25;
const qreal DocumentViewerWindow::ZOOM_MAX = 5.0;
const qreal DocumentViewerWindow::ZOOM_STEP = 0.1;
const qreal DocumentViewerWindow::ZOOM_DEFAULT = 1.0;

DocumentViewerWindow::DocumentViewerWindow(const QString& cartridgeName,
                                           const QString& cartridgePath,
                                           Theme::ThemeManager* themeManager,
                                           QWidget *parent)
    : QMainWindow(parent)
    , m_cartridgeName(cartridgeName)
    , m_cartridgePath(cartridgePath)
    , m_themeManager(themeManager)
    , m_webEngineView(nullptr)
    , m_webEngineBridge(nullptr)
    , m_zoomFactor(ZOOM_DEFAULT)
    , m_currentDocumentId()
{
    // Load saved zoom factor
    QSettings settings("CodexiumMagnus", "Settings");
    m_zoomFactor = settings.value(QString("viewer/%1/zoom").arg(cartridgePath), ZOOM_DEFAULT).toReal();
    
    setupUi();
    setupMenuBar();
    setupStatusBar();
    setupWebEngine();
    
    // Load saved window state
    loadWindowState();
    
    // Set unique window title (WCAG 2.4.2: Page Titled)
    setWindowTitle(QString("%1 - Codexium Magnus").arg(cartridgeName));
    
    // Ensure window is accessible
    setAttribute(Qt::WA_DeleteOnClose, false);  // We'll handle cleanup
}

DocumentViewerWindow::~DocumentViewerWindow() {
    saveWindowState();
}

void DocumentViewerWindow::setupUi() {
    // Central widget will be set in setupWebEngine()
}

void DocumentViewerWindow::setupMenuBar() {
    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Zoom In", this, &DocumentViewerWindow::onZoomIn, QKeySequence::ZoomIn);
    viewMenu->addAction("Zoom &Out", this, &DocumentViewerWindow::onZoomOut, QKeySequence::ZoomOut);
    viewMenu->addAction("&Normal Size", this, &DocumentViewerWindow::onZoomReset, QKeySequence("Ctrl+0"));
}

void DocumentViewerWindow::setupStatusBar() {
    statusBar()->showMessage("Ready");
    updateZoomDisplay();
}

void DocumentViewerWindow::setupWebEngine() {
    m_webEngineView = new QWebEngineView(this);
    setCentralWidget(m_webEngineView);
    
    // Set initial zoom factor
    m_webEngineView->setZoomFactor(m_zoomFactor);
    
    // Create WebEngine bridge
    m_webEngineBridge = new Services::WebEngineBridge(m_webEngineView, this);
    
    // Connect link clicks
    connect(m_webEngineView, &QWebEngineView::urlChanged,
            this, [this](const QUrl& url) {
                // Intercept external links
                if (url.scheme() == "http" || url.scheme() == "https" || 
                    url.scheme() == "mailto" || url.scheme() == "ftp") {
                    // Emit signal for MainWindow to handle
                    // For now, just prevent navigation
                    m_webEngineView->back();
                }
            });
}

void DocumentViewerWindow::loadDocument(const QString& documentId, const QString& content) {
    m_currentDocumentId = documentId;
    m_currentDocumentContent = content;
    
    QString wrappedContent = wrapContentWithTheme(content);
    m_webEngineView->setHtml(wrappedContent);
    
    statusBar()->showMessage(QString("Loaded: %1").arg(documentId), 2000);
}

void DocumentViewerWindow::updateTheme() {
    if (!m_currentDocumentContent.isEmpty()) {
        QString wrappedContent = wrapContentWithTheme(m_currentDocumentContent);
        m_webEngineView->setHtml(wrappedContent);
    }
}

void DocumentViewerWindow::setZoomFactor(qreal factor) {
    if (factor < ZOOM_MIN) factor = ZOOM_MIN;
    if (factor > ZOOM_MAX) factor = ZOOM_MAX;
    
    m_zoomFactor = factor;
    m_webEngineView->setZoomFactor(m_zoomFactor);
    updateZoomDisplay();
    
    // Save zoom preference
    QSettings settings("CodexiumMagnus", "Settings");
    settings.setValue(QString("viewer/%1/zoom").arg(m_cartridgePath), m_zoomFactor);
}

void DocumentViewerWindow::onZoomIn() {
    setZoomFactor(m_zoomFactor + ZOOM_STEP);
}

void DocumentViewerWindow::onZoomOut() {
    setZoomFactor(m_zoomFactor - ZOOM_STEP);
}

void DocumentViewerWindow::onZoomReset() {
    setZoomFactor(ZOOM_DEFAULT);
}

void DocumentViewerWindow::updateZoomDisplay() {
    int percent = static_cast<int>(m_zoomFactor * 100);
    statusBar()->showMessage(QString("Zoom: %1%").arg(percent), 0);
}

void DocumentViewerWindow::closeEvent(QCloseEvent* event) {
    saveWindowState();
    emit windowClosed(m_cartridgePath);
    event->accept();
}

void DocumentViewerWindow::loadWindowState() {
    QSettings settings("CodexiumMagnus", "Settings");
    
    // Load window geometry
    QString geometryKey = QString("viewer/%1/geometry").arg(m_cartridgePath);
    QByteArray geometry = settings.value(geometryKey).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    } else {
        // Default size
        resize(1000, 700);
    }
}

void DocumentViewerWindow::saveWindowState() {
    QSettings settings("CodexiumMagnus", "Settings");
    
    // Save window geometry
    QString geometryKey = QString("viewer/%1/geometry").arg(m_cartridgePath);
    settings.setValue(geometryKey, saveGeometry());
    
    // Save zoom
    settings.setValue(QString("viewer/%1/zoom").arg(m_cartridgePath), m_zoomFactor);
}

QString DocumentViewerWindow::wrapContentWithTheme(const QString& htmlContent) {
    if (!m_themeManager) {
        return htmlContent;
    }
    
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

} // namespace CodexiumMagnus::UI
