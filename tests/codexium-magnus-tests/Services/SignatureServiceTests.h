#ifndef SIGNATURESERVICETESTS_H
#define SIGNATURESERVICETESTS_H

#include <QtTest/QtTest>
#include <QSettings>

class SignatureServiceTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
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

private:
    // Helper methods
    QByteArray createValidPublicKey();
    QByteArray createValidSignature();
    QByteArray createManifestJson(const QByteArray& signature = QByteArray(), 
                                  const QByteArray& publicKey = QByteArray());
    QString createTestCartridgeFile(const QByteArray& manifest = QByteArray());
    void* createTestService(); // Returns SignatureService* - using void* to avoid include
    
    QSettings *m_testSettings;
};

#endif // SIGNATURESERVICETESTS_H
