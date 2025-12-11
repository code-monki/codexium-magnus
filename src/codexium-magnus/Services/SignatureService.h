#ifndef SIGNATURESERVICE_H
#define SIGNATURESERVICE_H

#include "ISignatureService.h"
#include <QMap>
#include <QSettings>

namespace CodexiumMagnus::Services {

/**
 * Implementation of ISignatureService using Ed25519 (via libsodium).
 * 
 * Provides cartridge signature verification and trust management.
 * Stores trusted keys in QSettings for persistence.
 */
class SignatureService : public ISignatureService {
    Q_OBJECT

public:
    explicit SignatureService(QObject *parent = nullptr);
    explicit SignatureService(QSettings *settings, QObject *parent = nullptr); // For testing
    ~SignatureService();

    TrustLevel verifyCartridge(const QString& cartridgePath) override;
    bool verifySignature(const QByteArray& message, 
                        const QByteArray& signature, 
                        const QByteArray& publicKey) override;
    void addTrustedKey(const QByteArray& publicKey, 
                      const QString& label, 
                      bool isOfficial = false) override;
    void removeTrustedKey(const QByteArray& publicKey) override;
    TrustLevel getKeyTrustLevel(const QByteArray& publicKey) const override;

    // Public methods for testing (can be made private with friend class if preferred)
    QByteArray extractSignature(const QByteArray& manifest);
    QByteArray extractPublicKey(const QByteArray& manifest);

private:
    /**
     * Compute hash of manifest + database for signature verification.
     * 
     * @param cartridgePath Path to the cartridge
     * @return Hash of manifest + database, or empty QByteArray on error
     */
    QByteArray computeCartridgeHash(const QString& cartridgePath);

    /**
     * Read manifest from cartridge.
     * 
     * @param cartridgePath Path to the cartridge
     * @return Manifest JSON as QByteArray, or empty on error
     */
    QByteArray readManifest(const QString& cartridgePath);

    /**
     * Initialize libsodium and load trusted keys.
     */
    void initialize();

    /**
     * Load trusted keys from QSettings.
     */
    void loadTrustedKeys();

    /**
     * Save trusted keys to QSettings.
     */
    void saveTrustedKeys();

    struct TrustedKey {
        QByteArray publicKey;
        QString label;
        bool isOfficial;
    };

    QMap<QByteArray, TrustedKey> m_trustedKeys;  ///< Map of public key -> key info
    QSettings *m_settings;                        ///< Settings for key persistence
    bool m_libsodiumAvailable;                    ///< Whether libsodium is available
};

} // namespace CodexiumMagnus::Services

#endif // SIGNATURESERVICE_H
