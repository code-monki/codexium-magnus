#ifndef ISIGNATURESERVICE_H
#define ISIGNATURESERVICE_H

#include <QObject>
#include <QString>
#include <QByteArray>

namespace CodexiumMagnus::Services {

/**
 * Trust classification for cartridges.
 */
enum class TrustLevel {
    Official,      ///< Signed by official publisher key
    Verified,      ///< Signed by verified publisher key
    Unverified,    ///< Signed but key not in trusted store
    Homebrew,      ///< Unsigned or invalid signature
    Invalid        ///< Tampered or corrupted
};

/**
 * Service interface for cartridge signature verification.
 * 
 * Implements FR-4: Cartridge Verification using Ed25519 signatures.
 * Verifies cartridge signatures and classifies trust levels.
 */
class ISignatureService : public QObject {
    Q_OBJECT

public:
    explicit ISignatureService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ISignatureService() = default;

    /**
     * Verify a cartridge signature.
     * 
     * Verifies the Ed25519 signature of a cartridge by:
     * 1. Reading the manifest from the cartridge
     * 2. Computing hash of manifest + database
     * 3. Verifying signature against known public keys
     * 
     * @param cartridgePath Path to the cartridge file
     * @return TrustLevel classification of the cartridge
     */
    virtual TrustLevel verifyCartridge(const QString& cartridgePath) = 0;

    /**
     * Verify a signature against a message and public key.
     * 
     * @param message The message that was signed
     * @param signature The Ed25519 signature (64 bytes)
     * @param publicKey The Ed25519 public key (32 bytes)
     * @return true if signature is valid, false otherwise
     */
    virtual bool verifySignature(const QByteArray& message, 
                                 const QByteArray& signature, 
                                 const QByteArray& publicKey) = 0;

    /**
     * Add a trusted public key.
     * 
     * @param publicKey The Ed25519 public key (32 bytes)
     * @param label Human-readable label for the key
     * @param isOfficial Whether this is an official publisher key
     */
    virtual void addTrustedKey(const QByteArray& publicKey, 
                               const QString& label, 
                               bool isOfficial = false) = 0;

    /**
     * Remove a trusted public key.
     * 
     * @param publicKey The Ed25519 public key to remove
     */
    virtual void removeTrustedKey(const QByteArray& publicKey) = 0;

    /**
     * Get trust level for a public key.
     * 
     * @param publicKey The Ed25519 public key
     * @return TrustLevel if key is trusted, Unverified otherwise
     */
    virtual TrustLevel getKeyTrustLevel(const QByteArray& publicKey) const = 0;

signals:
    /**
     * Emitted when a cartridge is verified.
     * @param cartridgePath Path to the cartridge
     * @param trustLevel The trust level determined
     */
    void cartridgeVerified(const QString& cartridgePath, TrustLevel trustLevel);

    /**
     * Emitted when signature verification fails.
     * @param cartridgePath Path to the cartridge
     * @param reason Description of the failure
     */
    void verificationFailed(const QString& cartridgePath, const QString& reason);
};

} // namespace CodexiumMagnus::Services

#endif // ISIGNATURESERVICE_H
