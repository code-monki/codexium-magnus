#ifndef ILINKSERVICE_H
#define ILINKSERVICE_H

#include <QObject>
#include <QUrl>
#include <QString>

namespace CodexiumMagnus::Services {

/**
 * Service interface for handling external links.
 * 
 * Mediates external URL navigation (FR-8). Intercepts link clicks from
 * Qt WebEngine content and determines whether to open them in the system
 * browser or block them based on security policies.
 * 
 * This service ensures that external links are handled securely and
 * according to user preferences, preventing navigation within the
 * WebEngine view for external URLs.
 */
class ILinkService : public QObject {
    Q_OBJECT

public:
    explicit ILinkService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ILinkService() = default;

    /**
     * Open an external link in the system's default browser.
     * 
     * This method checks if the URL should be opened externally using
     * shouldOpenExternally(), and if allowed, opens it via QDesktopServices.
     * 
     * @param url The URL to open
     */
    virtual void openExternalLink(const QUrl& url) = 0;

    /**
     * Check if a URL should be opened externally.
     * 
     * Determines whether the URL matches allowed schemes (http, https, mailto, etc.)
     * and is not in a blocked domain list.
     * 
     * @param url The URL to check
     * @return true if the URL should be opened externally, false if it should be blocked
     */
    virtual bool shouldOpenExternally(const QUrl& url) const = 0;

signals:
    /**
     * Emitted when an external link is successfully opened.
     * @param url The URL that was opened
     */
    void linkOpened(const QUrl& url);

    /**
     * Emitted when a link is blocked from opening.
     * @param url The URL that was blocked
     * @param reason Description of why the link was blocked
     */
    void linkBlocked(const QUrl& url, const QString& reason);
};

} // namespace CodexiumMagnus::Services

#endif // ILINKSERVICE_H