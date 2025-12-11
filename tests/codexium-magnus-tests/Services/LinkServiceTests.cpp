#include "LinkServiceTests.h"
#include <QUrl>
#include "Services/LinkService.h"

using namespace CodexiumMagnus::Services;

void LinkServiceTests::init() {
    m_service = new LinkService();
}

void LinkServiceTests::cleanup() {
    delete m_service;
    m_service = nullptr;
}

void LinkServiceTests::shouldOpenExternally_HttpUrl_ReturnsTrue() {
    QUrl url("http://example.com");
    QVERIFY(m_service->shouldOpenExternally(url));
}

void LinkServiceTests::shouldOpenExternally_HttpsUrl_ReturnsTrue() {
    QUrl url("https://example.com");
    QVERIFY(m_service->shouldOpenExternally(url));
}

void LinkServiceTests::shouldOpenExternally_MailtoUrl_ReturnsTrue() {
    QUrl url("mailto:test@example.com");
    QVERIFY(m_service->shouldOpenExternally(url));
}

void LinkServiceTests::shouldOpenExternally_FtpUrl_ReturnsTrue() {
    QUrl url("ftp://example.com");
    QVERIFY(m_service->shouldOpenExternally(url));
}

void LinkServiceTests::shouldOpenExternally_InternalUrl_ReturnsFalse() {
    QUrl url("cdoc://internal/document");
    QVERIFY(!m_service->shouldOpenExternally(url));
}

void LinkServiceTests::shouldOpenExternally_FileUrl_ReturnsFalse() {
    QUrl url("file:///path/to/file");
    QVERIFY(!m_service->shouldOpenExternally(url));
}

void LinkServiceTests::shouldOpenExternally_EmptyUrl_ReturnsFalse() {
    QUrl url;
    QVERIFY(!m_service->shouldOpenExternally(url));
}

void LinkServiceTests::shouldOpenExternally_BlockedDomain_ReturnsFalse() {
    m_service->addBlockedDomain("malicious.com");
    QUrl url("https://malicious.com/page");
    QVERIFY(!m_service->shouldOpenExternally(url));
}

void LinkServiceTests::addBlockedDomain_ValidDomain_BlocksDomain() {
    m_service->addBlockedDomain("example.com");
    QStringList blocked = m_service->getBlockedDomains();
    QVERIFY(blocked.contains("example.com"));
}

void LinkServiceTests::removeBlockedDomain_ExistingDomain_UnblocksDomain() {
    m_service->addBlockedDomain("example.com");
    m_service->removeBlockedDomain("example.com");
    QStringList blocked = m_service->getBlockedDomains();
    QVERIFY(!blocked.contains("example.com"));
}

void LinkServiceTests::getBlockedDomains_AfterAdding_ReturnsList() {
    m_service->addBlockedDomain("domain1.com");
    m_service->addBlockedDomain("domain2.com");
    QStringList blocked = m_service->getBlockedDomains();
    QCOMPARE(blocked.size(), 2);
    QVERIFY(blocked.contains("domain1.com"));
    QVERIFY(blocked.contains("domain2.com"));
}

void LinkServiceTests::getAllowedSchemes_Default_ReturnsStandardSchemes() {
    QStringList schemes = m_service->getAllowedSchemes();
    QVERIFY(schemes.contains("http"));
    QVERIFY(schemes.contains("https"));
    QVERIFY(schemes.contains("mailto"));
    QVERIFY(schemes.contains("ftp"));
}

void LinkServiceTests::addBlockedDomain_MultipleDomains_AllBlocked() {
    m_service->addBlockedDomain("bad1.com");
    m_service->addBlockedDomain("bad2.com");
    m_service->addBlockedDomain("bad3.com");
    
    QVERIFY(!m_service->shouldOpenExternally(QUrl("https://bad1.com")));
    QVERIFY(!m_service->shouldOpenExternally(QUrl("https://bad2.com")));
    QVERIFY(!m_service->shouldOpenExternally(QUrl("https://bad3.com")));
}

void LinkServiceTests::removeBlockedDomain_NonExistent_NoError() {
    // Should not crash or error
    m_service->removeBlockedDomain("nonexistent.com");
    QVERIFY(true); // If we get here, no error occurred
}

// QTEST_MAIN removed - using main.cpp instead
#include "LinkServiceTests.moc"
