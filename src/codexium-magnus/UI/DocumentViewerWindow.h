#ifndef DOCUMENTVIEWERWINDOW_H
#define DOCUMENTVIEWERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QMenuBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QSettings>
#include "../Services/WebEngineBridge.h"
#include "../Theme/ThemeManager.h"

namespace CodexiumMagnus::UI {

/**
 * Separate window for displaying cartridge document content.
 * 
 * Each cartridge gets its own viewer window, allowing multiple cartridges
 * to be open simultaneously. Provides WCAG-compliant document viewing
 * with independent zoom, theme, and window state management.
 */
class DocumentViewerWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit DocumentViewerWindow(const QString& cartridgeName, 
                                  const QString& cartridgePath,
                                  Theme::ThemeManager* themeManager,
                                  QWidget *parent = nullptr);
    ~DocumentViewerWindow();

    QString cartridgeName() const { return m_cartridgeName; }
    QString cartridgePath() const { return m_cartridgePath; }
    
    void loadDocument(const QString& documentId, const QString& content);
    void updateTheme();
    void setZoomFactor(qreal factor);
    qreal zoomFactor() const { return m_zoomFactor; }

signals:
    void windowClosed(const QString& cartridgePath);
    void documentRequested(const QString& documentId);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void updateZoomDisplay();

private:
    void setupUi();
    void setupMenuBar();
    void setupStatusBar();
    void setupWebEngine();
    void loadWindowState();
    void saveWindowState();
    QString wrapContentWithTheme(const QString& htmlContent);
    
    QString m_cartridgeName;
    QString m_cartridgePath;
    Theme::ThemeManager* m_themeManager;
    
    QWebEngineView* m_webEngineView;
    Services::WebEngineBridge* m_webEngineBridge;
    
    qreal m_zoomFactor;
    static const qreal ZOOM_MIN;
    static const qreal ZOOM_MAX;
    static const qreal ZOOM_STEP;
    static const qreal ZOOM_DEFAULT;
    
    QString m_currentDocumentContent;
    QString m_currentDocumentId;
};

} // namespace CodexiumMagnus::UI

#endif // DOCUMENTVIEWERWINDOW_H
