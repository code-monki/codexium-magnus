#include "WebEngineBridge.h"
#include <QWebEnginePage>
#include <QWebChannel>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

namespace CodexiumMagnus::Services {

WebEngineBridge::WebEngineBridge(QWebEngineView *webEngineView, QObject *parent)
    : QObject(parent)
    , m_webEngineView(webEngineView)
    , m_webChannel(nullptr)
{
    if (!m_webEngineView) {
        qWarning() << "WebEngineBridge: WebEngine view is null";
        return;
    }
    
    // Setup QWebChannel for C++ ↔ JavaScript communication
    m_webChannel = new QWebChannel(this);
    m_webChannel->registerObject("bridge", this);
    
    if (m_webEngineView->page()) {
        m_webEngineView->page()->setWebChannel(m_webChannel);
        
        // Note: Link interception is handled in MainWindow via urlChanged signal.
        // QWebEnginePage::linkClicked doesn't exist in Qt 6.
        // For more control, consider using a custom QWebEnginePage with acceptNavigationRequest().
    } else {
        qWarning() << "WebEngineBridge: WebEngine page is null";
    }
}

WebEngineBridge::~WebEngineBridge() {
    // QWebChannel and QWebEngineView are cleaned up by Qt's parent system
}

void WebEngineBridge::send(const QString& json) {
    if (!m_webEngineView || !m_webEngineView->page()) {
        qWarning() << "WebEngineBridge::send: WebEngine view or page not available";
        return;
    }

    // Validate JSON before sending
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "WebEngineBridge::send: Invalid JSON:" << error.errorString();
        return;
    }

    // Inject JavaScript to send message to the page
    // The page should have a handler: window.onHostMessage = function(json) { ... }
    QString script = QString(R"(
        (function() {
            if (typeof window.onHostMessage === 'function') {
                try {
                    var data = %1;
                    window.onHostMessage(data);
                } catch (e) {
                    console.error('Error in onHostMessage:', e);
                }
            } else {
                console.warn('onHostMessage handler not found');
            }
        })();
    )").arg(json);
    
    m_webEngineView->page()->runJavaScript(script);
}

void WebEngineBridge::onJavaScriptMessage(const QString& json) {
    // Validate JSON before emitting
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "WebEngineBridge::onJavaScriptMessage: Invalid JSON:" << error.errorString();
        return;
    }

    emit messageReceived(json);
}

} // namespace CodexiumMagnus::Services