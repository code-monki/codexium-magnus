#ifndef LINKSERVICETESTS_H
#define LINKSERVICETESTS_H

#include <QtTest/QtTest>

// Forward declaration with namespace
namespace CodexiumMagnus { namespace Services { class LinkService; } }

class LinkServiceTests : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    
    // External link detection tests
    void shouldOpenExternally_HttpUrl_ReturnsTrue();
    void shouldOpenExternally_HttpsUrl_ReturnsTrue();
    void shouldOpenExternally_MailtoUrl_ReturnsTrue();
    void shouldOpenExternally_FtpUrl_ReturnsTrue();
    void shouldOpenExternally_InternalUrl_ReturnsFalse();
    void shouldOpenExternally_FileUrl_ReturnsFalse();
    void shouldOpenExternally_EmptyUrl_ReturnsFalse();
    
    // Blocked domain tests
    void shouldOpenExternally_BlockedDomain_ReturnsFalse();
    void addBlockedDomain_ValidDomain_BlocksDomain();
    void removeBlockedDomain_ExistingDomain_UnblocksDomain();
    void getBlockedDomains_AfterAdding_ReturnsList();
    
    // Allowed schemes tests
    void getAllowedSchemes_Default_ReturnsStandardSchemes();
    
    // Integration tests
    void addBlockedDomain_MultipleDomains_AllBlocked();
    void removeBlockedDomain_NonExistent_NoError();

private:
    CodexiumMagnus::Services::LinkService* m_service;
};

#endif // LINKSERVICETESTS_H
