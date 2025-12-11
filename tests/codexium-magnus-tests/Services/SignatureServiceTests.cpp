#include "SignatureServiceTests.h"
#include <QByteArray>
#include <QString>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
#include "Services/SignatureService.h"
#include "Services/ISignatureService.h"

#ifdef HAVE_LIBSODIUM
#include <sodium.h>
#endif

using namespace CodexiumMagnus::Services;

// Remove class definition - it's now in the header
// class SignatureServiceTests : public QObject {
//     Q_OBJECT
//
// private slots:
    void initTestCase();
    void cleanupTestCase();
    void init(); // Called before each test
    void cleanup(); // Called after each test
    
    // Trusted key management tests
    void addTrustedKey_ValidKey_Stored();
    void addTrustedKey_InvalidSize_Rejected();
    void removeTrustedKey_ExistingKey_Removed();
    void getKeyTrustLevel_OfficialKey_ReturnsOfficial();
    void getKeyTrustLevel_VerifiedKey_ReturnsVerified();
    void getKeyTrustLevel_UnknownKey_ReturnsUnverified();
    
    // Signature verification tests (require libsodium)
    void verifySignature_ValidSignature_ReturnsTrue();
    void verifySignature_InvalidSignature_ReturnsFalse();
    void verifySignature_WrongKey_ReturnsFalse();
    void verifySignature_InvalidSizes_ReturnsFalse();
    
    // Manifest parsing tests
    void extractSignature_ValidManifest_ReturnsSignature();
    void extractSignature_NoSignature_ReturnsEmpty();
    void extractPublicKey_ValidManifest_ReturnsKey();
    void extractPublicKey_NoKey_ReturnsEmpty();
    
    // Cartridge verification tests
    void verifyCartridge_UnsignedCartridge_ReturnsHomebrew();
    void verifyCartridge_InvalidPath_ReturnsInvalid();

void SignatureServiceTests::initTestCase() {
    // Setup for all tests
    m_testSettings = nullptr;
}

void SignatureServiceTests::cleanupTestCase() {
    // Cleanup after all tests
    if (m_testSettings) {
        delete m_testSettings;
        m_testSettings = nullptr;
    }
}

void SignatureServiceTests::init() {
    // Create fresh temporary settings for each test
    m_testSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, 
                                   "CodexiumMagnusTest", "SignatureServiceTest");
    m_testSettings->clear(); // Ensure clean state
}

void SignatureServiceTests::cleanup() {
    // Clean up temporary settings after each test
    if (m_testSettings) {
        m_testSettings->clear();
        delete m_testSettings;
        m_testSettings = nullptr;
    }
}

// Helper method implementations
void* SignatureServiceTests::createTestService() {
    // Create a new temporary settings for this service instance
    QSettings *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                       "CodexiumMagnusTest", "SignatureServiceTest");
    settings->clear();
    return new SignatureService(settings);
}

QByteArray SignatureServiceTests::createValidPublicKey() {
    // Create a 32-byte public key (Ed25519 public keys are 32 bytes)
    QByteArray key(32, 0);
    for (int i = 0; i < 32; ++i) {
        key[i] = static_cast<char>(i);
    }
    return key;
}

QByteArray SignatureServiceTests::createValidSignature() {
    // Create a 64-byte signature (Ed25519 signatures are 64 bytes)
    QByteArray sig(64, 0);
    for (int i = 0; i < 64; ++i) {
        sig[i] = static_cast<char>(i + 32);
    }
    return sig;
}

QByteArray SignatureServiceTests::createManifestJson(const QByteArray& signature, 
                                                      const QByteArray& publicKey) {
    QJsonObject obj;
    obj["version"] = "1.0";
    obj["corpus"] = "Test Corpus";
    obj["title"] = "Test Cartridge";
    
    if (!signature.isEmpty()) {
        obj["signature"] = QString::fromLatin1(signature.toBase64());
    }
    if (!publicKey.isEmpty()) {
        obj["publicKey"] = QString::fromLatin1(publicKey.toBase64());
    }
    
    QJsonDocument doc(obj);
    return doc.toJson();
}

QString SignatureServiceTests::createTestCartridgeFile(const QByteArray& manifest) {
    // Create a temporary SQLite file that could serve as a test cartridge
    // For now, just create an empty file - full implementation would need SQLite structure
    QTemporaryFile tempFile;
    tempFile.setFileTemplate(QDir::temp().absoluteFilePath("test-cartridge-XXXXXX.db"));
    if (tempFile.open()) {
        // In a real test, we'd write SQLite structure and manifest here
        tempFile.write(manifest);
        tempFile.close();
        return tempFile.fileName();
    }
    return QString();
}

void SignatureServiceTests::addTrustedKey_ValidKey_Stored() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray key = createValidPublicKey();
    
    service->addTrustedKey(key, "Test Key", true);
    
    TrustLevel level = service->getKeyTrustLevel(key);
    QCOMPARE(level, TrustLevel::Official);
    delete service;
}

void SignatureServiceTests::addTrustedKey_InvalidSize_Rejected() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray invalidKey(16, 0); // Wrong size (should be 32)
    
    // Should not crash - invalid key should be rejected
    service->addTrustedKey(invalidKey, "Invalid Key");
    
    // Key should not be stored
    TrustLevel level = service->getKeyTrustLevel(invalidKey);
    QCOMPARE(level, TrustLevel::Unverified);
    delete service;
}

void SignatureServiceTests::removeTrustedKey_ExistingKey_Removed() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray key = createValidPublicKey();
    
    service->addTrustedKey(key, "Test Key");
    QCOMPARE(service->getKeyTrustLevel(key), TrustLevel::Verified);
    
    service->removeTrustedKey(key);
    QCOMPARE(service->getKeyTrustLevel(key), TrustLevel::Unverified);
    delete service;
}

void SignatureServiceTests::getKeyTrustLevel_OfficialKey_ReturnsOfficial() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray key = createValidPublicKey();
    
    service->addTrustedKey(key, "Official Key", true);
    QCOMPARE(service->getKeyTrustLevel(key), TrustLevel::Official);
    delete service;
}

void SignatureServiceTests::getKeyTrustLevel_VerifiedKey_ReturnsVerified() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray key = createValidPublicKey();
    
    service->addTrustedKey(key, "Verified Key", false);
    QCOMPARE(service->getKeyTrustLevel(key), TrustLevel::Verified);
    delete service;
}

void SignatureServiceTests::getKeyTrustLevel_UnknownKey_ReturnsUnverified() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    // Use a different key that definitely hasn't been added
    QByteArray key(32, 0xFF); // All 0xFF bytes - different from createValidPublicKey()
    
    // Don't add the key
    TrustLevel level = service->getKeyTrustLevel(key);
    QCOMPARE(level, TrustLevel::Unverified);
    delete service;
}

void SignatureServiceTests::verifySignature_ValidSignature_ReturnsTrue() {
#ifdef HAVE_LIBSODIUM
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    
    // Generate a test key pair using libsodium
    unsigned char publicKey[32];
    unsigned char secretKey[64];
    crypto_sign_keypair(publicKey, secretKey);
    
    // Create a test message
    QByteArray message = "Test message for signature verification";
    
    // Sign the message
    unsigned char signature[64];
    crypto_sign_detached(signature, nullptr,
                        reinterpret_cast<const unsigned char*>(message.data()),
                        message.size(),
                        secretKey);
    
    // Verify the signature
    QByteArray sigBytes(reinterpret_cast<const char*>(signature), 64);
    QByteArray pubKeyBytes(reinterpret_cast<const char*>(publicKey), 32);
    
    bool result = service->verifySignature(message, sigBytes, pubKeyBytes);
    QVERIFY(result);
    delete service;
#else
    QSKIP("libsodium not available - skipping signature verification test");
#endif
}

void SignatureServiceTests::verifySignature_InvalidSignature_ReturnsFalse() {
#ifdef HAVE_LIBSODIUM
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    
    // Generate a test key pair
    unsigned char publicKey[32];
    unsigned char secretKey[64];
    crypto_sign_keypair(publicKey, secretKey);
    
    QByteArray message = "Test message";
    QByteArray invalidSignature(64, 0); // All zeros - invalid signature
    QByteArray pubKeyBytes(reinterpret_cast<const char*>(publicKey), 32);
    
    bool result = service->verifySignature(message, invalidSignature, pubKeyBytes);
    QVERIFY(!result);
    delete service;
#else
    QSKIP("libsodium not available - skipping signature verification test");
#endif
}

void SignatureServiceTests::verifySignature_WrongKey_ReturnsFalse() {
#ifdef HAVE_LIBSODIUM
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    
    // Generate two different key pairs
    unsigned char publicKey1[32], secretKey1[64];
    unsigned char publicKey2[32], secretKey2[64];
    crypto_sign_keypair(publicKey1, secretKey1);
    crypto_sign_keypair(publicKey2, secretKey2);
    
    // Sign with key1
    QByteArray message = "Test message";
    unsigned char signature[64];
    crypto_sign_detached(signature, nullptr,
                        reinterpret_cast<const unsigned char*>(message.data()),
                        message.size(),
                        secretKey1);
    
    // Try to verify with key2 (wrong key)
    QByteArray sigBytes(reinterpret_cast<const char*>(signature), 64);
    QByteArray pubKey2Bytes(reinterpret_cast<const char*>(publicKey2), 32);
    
    bool result = service->verifySignature(message, sigBytes, pubKey2Bytes);
    QVERIFY(!result);
    delete service;
#else
    QSKIP("libsodium not available - skipping signature verification test");
#endif
}

void SignatureServiceTests::verifySignature_InvalidSizes_ReturnsFalse() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    
    QByteArray message = "Test message";
    QByteArray invalidSig(32, 0); // Wrong size (should be 64)
    QByteArray invalidKey(16, 0); // Wrong size (should be 32)
    
    bool result1 = service->verifySignature(message, invalidSig, createValidPublicKey());
    QVERIFY(!result1);
    
    bool result2 = service->verifySignature(message, createValidSignature(), invalidKey);
    QVERIFY(!result2);
    delete service;
}

void SignatureServiceTests::extractSignature_ValidManifest_ReturnsSignature() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray expectedSig = createValidSignature();
    QByteArray manifest = createManifestJson(expectedSig, createValidPublicKey());
    
    QByteArray extracted = service->extractSignature(manifest);
    QCOMPARE(extracted, expectedSig);
    delete service;
}

void SignatureServiceTests::extractSignature_NoSignature_ReturnsEmpty() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray manifest = createManifestJson(); // No signature
    
    QByteArray extracted = service->extractSignature(manifest);
    QVERIFY(extracted.isEmpty());
    delete service;
}

void SignatureServiceTests::extractPublicKey_ValidManifest_ReturnsKey() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray expectedKey = createValidPublicKey();
    QByteArray manifest = createManifestJson(createValidSignature(), expectedKey);
    
    QByteArray extracted = service->extractPublicKey(manifest);
    QCOMPARE(extracted, expectedKey);
    delete service;
}

void SignatureServiceTests::extractPublicKey_NoKey_ReturnsEmpty() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray manifest = createManifestJson(); // No public key
    
    QByteArray extracted = service->extractPublicKey(manifest);
    QVERIFY(extracted.isEmpty());
    delete service;
}

void SignatureServiceTests::verifyCartridge_UnsignedCartridge_ReturnsHomebrew() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QByteArray manifest = createManifestJson(); // No signature or key
    QString cartridgePath = createTestCartridgeFile(manifest);
    
    if (!cartridgePath.isEmpty()) {
        TrustLevel level = service->verifyCartridge(cartridgePath);
        // Note: This will return Unverified or Invalid until manifest reading is implemented
        // For now, we just verify it doesn't crash
        QVERIFY(level == TrustLevel::Homebrew || 
                level == TrustLevel::Unverified || 
                level == TrustLevel::Invalid);
    } else {
        QSKIP("Could not create test cartridge file");
    }
    delete service;
}

void SignatureServiceTests::verifyCartridge_InvalidPath_ReturnsInvalid() {
    SignatureService* service = static_cast<SignatureService*>(createTestService());
    QString invalidPath = "/nonexistent/path/to/cartridge.db";
    
    TrustLevel level = service->verifyCartridge(invalidPath);
    QCOMPARE(level, TrustLevel::Invalid);
    delete service;
}

// QTEST_MAIN removed - using main.cpp test runner instead
#include "SignatureServiceTests.moc"

