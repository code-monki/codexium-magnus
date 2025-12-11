#include "LinkService.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QDebug>

namespace CodexiumMagnus::Services {

LinkService::LinkService(QObject *parent)
    : ILinkService(parent)
{
    // Default allowed schemes
    m_allowedSchemes << "http" << "https" << "mailto" << "ftp";
    
    // Blocked domains (can be configured via settings)
    // m_blockedDomains << "example-blocked-domain.com";
}

LinkService::~LinkService() {
    // No cleanup needed - Qt parent system handles QObject destruction
}

void LinkService::openExternalLink(const QUrl& url) {
    if (!url.isValid()) {
        emit linkBlocked(url, "Invalid URL");
        return;
    }

    if (!shouldOpenExternally(url)) {
        emit linkBlocked(url, "Link scheme or domain is not allowed");
        return;
    }

    if (QDesktopServices::openUrl(url)) {
        emit linkOpened(url);
    } else {
        emit linkBlocked(url, "Failed to open link");
    }
}

bool LinkService::shouldOpenExternally(const QUrl& url) const {
    if (!url.isValid()) {
        return false;
    }

    // Check scheme
    QString scheme = url.scheme().toLower();
    if (!m_allowedSchemes.contains(scheme)) {
        return false;
    }

    // Check blocked domains
    QString host = url.host().toLower();
    for (const QString& blocked : m_blockedDomains) {
        if (host.contains(blocked, Qt::CaseInsensitive)) {
            return false;
        }
    }

    return true;
}

void LinkService::addBlockedDomain(const QString& domain) {
    if (domain.isEmpty()) {
        return;
    }

    QString normalizedDomain = domain.toLower().trimmed();
    if (!m_blockedDomains.contains(normalizedDomain, Qt::CaseInsensitive)) {
        m_blockedDomains.append(normalizedDomain);
    }
}

void LinkService::removeBlockedDomain(const QString& domain) {
    if (domain.isEmpty()) {
        return;
    }

    QString normalizedDomain = domain.toLower().trimmed();
    m_blockedDomains.removeAll(normalizedDomain);
}

} // namespace CodexiumMagnus::Services