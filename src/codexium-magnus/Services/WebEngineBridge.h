#ifndef WEBENGINEBRIDGE_H
#define WEBENGINEBRIDGE_H

#include <QObject>
#include <QString>
#include <QWebEngineView>
#include <QWebChannel>
#include <QUrl>

namespace CodexiumMagnus::Services {

/**
 * Bridge interface for communicating with the embedded Qt WebEngine.
 * 
 * Replaces the CEF bridge from the Avalonia version. Provides bidirectional
 * communication between C++ code and JavaScript running in Qt WebEngine.
 * Uses QWebChannel for reliable message passing.
 * 
 * The bridge allows:
 * - Sending messages from C++ to JavaScript
 * - Receiving messages from JavaScript to C++
 * - Intercepting link clicks for external link handling
 */
class WebEngineBridge : public QObject {
    Q_OBJECT

public:
    /**
     * Construct a new WebEngineBridge instance.
     * 
     * Sets up QWebChannel for communication and registers this object
     * as "bridge" in the JavaScript context.
     * 
     * @param webEngineView The QWebEngineView to bridge with
     * @param parent Parent QObject for memory management
     */
    explicit WebEngineBridge(QWebEngineView *webEngineView, QObject *parent = nullptr);
    
    /**
     * Destructor.
     */
    ~WebEngineBridge();

    /**
     * Send a JSON message to JavaScript in the WebEngine page.
     * 
     * The message is sent via JavaScript injection. The page should
     * have a handler function `window.onHostMessage(json)` to receive it.
     * 
     * @param json JSON string to send to JavaScript
     */
    void send(const QString& json);

signals:
    /**
     * Emitted when a message is received from JavaScript.
     * 
     * JavaScript can send messages by calling:
     * window.bridge.onJavaScriptMessage(jsonString)
     * 
     * @param json JSON string received from JavaScript
     */
    void messageReceived(const QString& json);

    /**
     * Emitted when a link is clicked in the WebEngine content.
     * 
     * This signal is emitted for all link clicks, allowing the
     * application to decide whether to handle them internally or
     * open them externally.
     * 
     * @param url The URL that was clicked
     */
    void linkClicked(const QUrl& url);

private slots:
    /**
     * Handle messages received from JavaScript via QWebChannel.
     * 
     * @param json JSON string from JavaScript
     */
    void onJavaScriptMessage(const QString& json);

private:
    QWebEngineView *m_webEngineView;  ///< The WebEngine view to bridge with
    QWebChannel *m_webChannel;        ///< QWebChannel for communication
};

} // namespace CodexiumMagnus::Services

#endif // WEBENGINEBRIDGE_H