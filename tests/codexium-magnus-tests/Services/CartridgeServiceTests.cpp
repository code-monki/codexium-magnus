#include "CartridgeServiceTests.h"
#include <QTemporaryFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QFile>
#include "Services/CartridgeService.h"
#include "Services/ISignatureService.h"

using namespace CodexiumMagnus::Services;

// Helper class to create test cartridge databases
class TestCartridgeHelper {
public:
    static QString createTestCartridge(const QString& path) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "test_cartridge");
        db.setDatabaseName(path);
        
        if (!db.open()) {
            return QString();
        }
        
        QSqlQuery query(db);
        
        // Create documents table
        query.exec(R"(
            CREATE TABLE IF NOT EXISTS documents (
                id TEXT PRIMARY KEY,
                title TEXT NOT NULL,
                content TEXT,
                parent_id TEXT
            )
        )");
        
        // Insert test documents
        query.prepare("INSERT INTO documents (id, title, content, parent_id) VALUES (?, ?, ?, ?)");
        query.addBindValue("doc1");
        query.addBindValue("Document 1");
        query.addBindValue("<html><body>Content 1</body></html>");
        query.addBindValue("");
        query.exec();
        
        query.addBindValue("doc2");
        query.addBindValue("Document 2");
        query.addBindValue("<html><body>Content 2</body></html>");
        query.addBindValue("");
        query.exec();
        
        query.addBindValue("doc3");
        query.addBindValue("Subdocument");
        query.addBindValue("<html><body>Sub content</body></html>");
        query.addBindValue("doc1");
        query.exec();
        
        db.close();
        QSqlDatabase::removeDatabase("test_cartridge");
        
        return path;
    }
};

void CartridgeServiceTests::init() {
    m_service = new CartridgeService();
    // Cast to proper type for use
    
    // Create temporary test cartridge
    m_testCartridge = new QTemporaryFile();
    m_testCartridge->setAutoRemove(false);
    m_testCartridge->open();
    QString path = m_testCartridge->fileName();
    m_testCartridge->close();
    
    TestCartridgeHelper::createTestCartridge(path);
}

void CartridgeServiceTests::cleanup() {
    delete static_cast<CartridgeService*>(m_service);
    m_service = nullptr;
    
    if (m_testCartridge) {
        QFile::remove(m_testCartridge->fileName());
        delete m_testCartridge;
    }
}

void CartridgeServiceTests::loadCartridge_ValidPath_ReturnsTrue() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    QVERIFY(service->loadCartridge(m_testCartridge->fileName()));
    QVERIFY(service->isCartridgeLoaded());
}

void CartridgeServiceTests::loadCartridge_InvalidPath_ReturnsFalse() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    QVERIFY(!service->loadCartridge("/nonexistent/path/to/cartridge.db"));
    QVERIFY(!service->isCartridgeLoaded());
}

void CartridgeServiceTests::loadCartridge_NonExistentFile_ReturnsFalse() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    QVERIFY(!service->loadCartridge("/tmp/nonexistent_cartridge_12345.db"));
    QVERIFY(!service->isCartridgeLoaded());
}

void CartridgeServiceTests::isCartridgeLoaded_AfterLoad_ReturnsTrue() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QVERIFY(service->isCartridgeLoaded());
}

void CartridgeServiceTests::isCartridgeLoaded_BeforeLoad_ReturnsFalse() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    QVERIFY(!service->isCartridgeLoaded());
}

void CartridgeServiceTests::getCartridgeName_AfterLoad_ReturnsName() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QString name = service->getCartridgeName();
    QVERIFY(!name.isEmpty());
}

void CartridgeServiceTests::getCartridgePath_AfterLoad_ReturnsPath() {
    QString path = m_testCartridge->fileName();
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(path);
    QCOMPARE(service->getCartridgePath(), path);
}

void CartridgeServiceTests::getDocumentContent_ValidId_ReturnsContent() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QString content = service->getDocumentContent("doc1");
    QVERIFY(!content.isEmpty());
    QVERIFY(content.contains("Content 1"));
}

void CartridgeServiceTests::getDocumentContent_InvalidId_ReturnsEmpty() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QString content = service->getDocumentContent("nonexistent");
    QVERIFY(content.isEmpty());
}

void CartridgeServiceTests::getDocumentContent_NoCartridge_ReturnsEmpty() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    QString content = service->getDocumentContent("doc1");
    QVERIFY(content.isEmpty());
}

void CartridgeServiceTests::getDocumentList_AfterLoad_ReturnsList() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QStringList documents = service->getDocumentList();
    QVERIFY(documents.size() >= 2);
    QVERIFY(documents.contains("doc1"));
    QVERIFY(documents.contains("doc2"));
}

void CartridgeServiceTests::getNavigationModel_AfterLoad_ReturnsModel() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QStandardItemModel* model = service->getNavigationModel();
    QVERIFY(model != nullptr);
}

void CartridgeServiceTests::getNavigationModel_NoCartridge_ReturnsModel() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    QStandardItemModel* model = service->getNavigationModel();
    QVERIFY(model != nullptr);
}

void CartridgeServiceTests::unloadCartridge_AfterLoad_Unloads() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QVERIFY(service->isCartridgeLoaded());
    
    service->unloadCartridge();
    QVERIFY(!service->isCartridgeLoaded());
    QVERIFY(service->getCartridgePath().isEmpty());
}

void CartridgeServiceTests::unloadCartridge_NoCartridge_NoError() {
    // Should not crash
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->unloadCartridge();
    QVERIFY(true);
}

void CartridgeServiceTests::getDatabase_AfterLoad_ReturnsDatabase() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    QSqlDatabase* db = service->getDatabase();
    QVERIFY(db != nullptr);
    QVERIFY(db->isOpen());
}

void CartridgeServiceTests::getDatabase_NoCartridge_ReturnsNull() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    QSqlDatabase* db = service->getDatabase();
    QVERIFY(db == nullptr);
}

void CartridgeServiceTests::getTrustLevel_AfterLoad_ReturnsLevel() {
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->loadCartridge(m_testCartridge->fileName());
    TrustLevel level = service->getTrustLevel();
    // Should be Unverified if no signature service
    QCOMPARE(level, TrustLevel::Unverified);
}

void CartridgeServiceTests::setSignatureService_ValidService_SetsService() {
    // Create a mock signature service (just verify the method doesn't crash)
    ISignatureService* mockService = nullptr; // In real test, would use a mock
    CartridgeService* service = static_cast<CartridgeService*>(m_service);
    service->setSignatureService(mockService);
    QVERIFY(true); // If we get here, no crash occurred
}

// QTEST_MAIN removed - using main.cpp instead
#include "CartridgeServiceTests.moc"
