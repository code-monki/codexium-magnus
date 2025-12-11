#ifndef CARTRIDGESERVICETESTS_H
#define CARTRIDGESERVICETESTS_H

#include <QtTest/QtTest>
#include <QTemporaryFile>

class CartridgeServiceTests : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    
    // Cartridge loading tests
    void loadCartridge_ValidPath_ReturnsTrue();
    void loadCartridge_InvalidPath_ReturnsFalse();
    void loadCartridge_NonExistentFile_ReturnsFalse();
    void isCartridgeLoaded_AfterLoad_ReturnsTrue();
    void isCartridgeLoaded_BeforeLoad_ReturnsFalse();
    
    // Cartridge info tests
    void getCartridgeName_AfterLoad_ReturnsName();
    void getCartridgePath_AfterLoad_ReturnsPath();
    
    // Document access tests
    void getDocumentContent_ValidId_ReturnsContent();
    void getDocumentContent_InvalidId_ReturnsEmpty();
    void getDocumentContent_NoCartridge_ReturnsEmpty();
    void getDocumentList_AfterLoad_ReturnsList();
    
    // Navigation model tests
    void getNavigationModel_AfterLoad_ReturnsModel();
    void getNavigationModel_NoCartridge_ReturnsModel();
    
    // Unload tests
    void unloadCartridge_AfterLoad_Unloads();
    void unloadCartridge_NoCartridge_NoError();
    
    // Database access tests
    void getDatabase_AfterLoad_ReturnsDatabase();
    void getDatabase_NoCartridge_ReturnsNull();
    
    // Trust level tests
    void getTrustLevel_AfterLoad_ReturnsLevel();
    void setSignatureService_ValidService_SetsService();

private:
    void* m_service; // CartridgeService* - using void* to avoid include in header
    QTemporaryFile* m_testCartridge;
};

#endif // CARTRIDGESERVICETESTS_H
