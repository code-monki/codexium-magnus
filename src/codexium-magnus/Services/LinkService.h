#ifndef LINKSERVICE_H
#define LINKSERVICE_H

#include "ILinkService.h"
#include <QUrl>
#include <QStringList>

namespace CodexiumMagnus::Services {

/**
 * Implementation of ILinkService using QDesktopServices.
 * 
 * Handles external link navigation by opening URLs in the system's default
 * browser or appropriate application. Implements security policies to allow
 * or block specific URL schemes and domains.
 * 
 * Default allowed schemes: http, https, mailto, ftp
 * Blocked domains can be configured via settings or programmatically.
 */
class LinkService : public ILinkService {
    Q_OBJECT

public:
    /**
     * Construct a new LinkService instance.
     * 
     * Initializes the service with default allowed schemes and empty
     * blocked domains list.
     * 
     * @param parent Parent QObject for memory management
     */
    explicit LinkService(QObject *parent = nullptr);
    
    /**
     * Destructor.
     */
    ~LinkService();

    /**
     * Open an external link in the system's default browser.
     * 
     * Checks if the URL should be opened externally, and if allowed,
     * opens it via QDesktopServices::openUrl().
     * 
     * @param url The URL to open
     */
    void openExternalLink(const QUrl& url) override;

    /**
     * Check if a URL should be opened externally.
     * 
     * Validates the URL against allowed schemes and blocked domains.
     * 
     * @param url The URL to check
     * @return true if the URL should be opened externally, false if blocked
     */
    bool shouldOpenExternally(const QUrl& url) const override;

    /**
     * Add a domain to the blocked list.
     * 
     * @param domain Domain name to block (e.g., "example.com")
     */
    void addBlockedDomain(const QString& domain);

    /**
     * Remove a domain from the blocked list.
     * 
     * @param domain Domain name to unblock
     */
    void removeBlockedDomain(const QString& domain);

    /**
     * Get the list of blocked domains.
     * 
     * @return List of blocked domain names
     */
    QStringList getBlockedDomains() const { return m_blockedDomains; }

    /**
     * Get the list of allowed URL schemes.
     * 
     * @return List of allowed scheme names (e.g., "http", "https")
     */
    QStringList getAllowedSchemes() const { return m_allowedSchemes; }

private:
    QStringList m_allowedSchemes;   ///< List of allowed URL schemes
    QStringList m_blockedDomains;   ///< List of blocked domain names
};

} // namespace CodexiumMagnus::Services

#endif // LINKSERVICE_H