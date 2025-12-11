#ifndef CARTRIDGESERVICE_H
#define CARTRIDGESERVICE_H

#include "ICartridgeService.h"
#include "ISignatureService.h"
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QString>

namespace CodexiumMagnus::Services {

/**
 * Implementation of ICartridgeService.
 * Loads SQLite cartridges and provides navigation/content access.
 */
class CartridgeService : public ICartridgeService {
    Q_OBJECT

public:
    explicit CartridgeService(QObject *parent = nullptr);
    ~CartridgeService();

    bool loadCartridge(const QString& path) override;
    void unloadCartridge() override;
    bool isCartridgeLoaded() const override;
    QString getCartridgeName() const override;
    QString getCartridgePath() const override;
    
    QStandardItemModel* getNavigationModel() override;
    QString getDocumentContent(const QString& documentId) override;
    QStringList getDocumentList() const override;
    
    // Expose database connection for services that need direct access (e.g., FTS5 search)
    QSqlDatabase* getDatabase() const;
    
    // Get trust level of currently loaded cartridge
    TrustLevel getTrustLevel() const { return m_trustLevel; }
    
    // Set signature service for cartridge verification
    void setSignatureService(ISignatureService *service) { m_signatureService = service; }

signals:
    /**
     * Emitted when cartridge trust level is determined.
     * @param trustLevel The trust level of the cartridge
     */
    void trustLevelDetermined(TrustLevel trustLevel);

private:
    void buildNavigationModel();
    QString queryDocumentContent(const QString& documentId);

    QString m_cartridgePath;
    QString m_cartridgeName;
    QSqlDatabase m_database;
    QStandardItemModel *m_navigationModel;
    bool m_isLoaded;
    TrustLevel m_trustLevel;
    ISignatureService *m_signatureService;  ///< Signature verification service
};

} // namespace CodexiumMagnus::Services

#endif // CARTRIDGESERVICE_H