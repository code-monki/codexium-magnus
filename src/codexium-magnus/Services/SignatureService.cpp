#include "SignatureService.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

#ifdef HAVE_LIBSODIUM
#include <sodium.h>
#endif

namespace CodexiumMagnus::Services {

SignatureService::SignatureService(QObject *parent)
    : ISignatureService(parent)
    , m_settings(nullptr)
    , m_libsodiumAvailable(false)
{
    m_settings = new QSettings("CodexiumMagnus", "SignatureService", this);
    initialize();
}

SignatureService::SignatureService(QSettings *settings, QObject *parent)
    : ISignatureService(parent)
    , m_settings(settings)
    , m_libsodiumAvailable(false)
{
    // Use provided settings (for testing) - caller manages lifetime
    initialize();
}

void SignatureService::initialize() {
#ifdef HAVE_LIBSODIUM
    if (sodium_init() < 0) {
        qWarning() << "SignatureService: Failed to initialize libsodium";
        m_libsodiumAvailable = false;
    } else {
        m_libsodiumAvailable = true;
        qDebug() << "SignatureService: libsodium initialized successfully";
    }
#else
    m_libsodiumAvailable = false;
    qWarning() << "SignatureService: libsodium not available - signature verification disabled";
#endif
    
    if (m_settings) {
        loadTrustedKeys();
    }
}

SignatureService::~SignatureService() {
    // Qt parent system handles cleanup
}

TrustLevel SignatureService::verifyCartridge(const QString& cartridgePath) {
    if (!QFile::exists(cartridgePath)) {
        emit verificationFailed(cartridgePath, "Cartridge file does not exist");
        return TrustLevel::Invalid;
    }

    // Read manifest
    QByteArray manifest = readManifest(cartridgePath);
    if (manifest.isEmpty()) {
        emit verificationFailed(cartridgePath, "Failed to read manifest");
        return TrustLevel::Invalid;
    }

    // Extract signature and public key from manifest
    QByteArray signature = extractSignature(manifest);
    QByteArray publicKey = extractPublicKey(manifest);

    if (signature.isEmpty() || publicKey.isEmpty()) {
        // Unsigned cartridge
        emit cartridgeVerified(cartridgePath, TrustLevel::Homebrew);
        return TrustLevel::Homebrew;
    }

    // Compute hash of manifest + database
    QByteArray message = computeCartridgeHash(cartridgePath);
    if (message.isEmpty()) {
        emit verificationFailed(cartridgePath, "Failed to compute cartridge hash");
        return TrustLevel::Invalid;
    }

    // Verify signature
    if (!m_libsodiumAvailable) {
        qWarning() << "SignatureService: libsodium not available, cannot verify signature";
        emit verificationFailed(cartridgePath, "Signature verification library not available");
        return TrustLevel::Unverified;
    }

#ifdef HAVE_LIBSODIUM
    bool isValid = verifySignature(message, signature, publicKey);
    if (!isValid) {
        emit verificationFailed(cartridgePath, "Invalid signature");
        return TrustLevel::Invalid;
    }
#else
    // libsodium not available - cannot verify signature
    emit verificationFailed(cartridgePath, "Signature verification library not available");
    return TrustLevel::Unverified;
#endif

    // Check if key is trusted
    TrustLevel trustLevel = getKeyTrustLevel(publicKey);
    emit cartridgeVerified(cartridgePath, trustLevel);
    return trustLevel;
}

bool SignatureService::verifySignature(const QByteArray& message, 
                                       const QByteArray& signature, 
                                       const QByteArray& publicKey) {
    if (!m_libsodiumAvailable) {
        return false;
    }

    if (signature.size() != 64 || publicKey.size() != 32) {
        qWarning() << "SignatureService: Invalid signature or public key size";
        return false;
    }

#ifdef HAVE_LIBSODIUM
    return crypto_sign_verify_detached(
        reinterpret_cast<const unsigned char*>(signature.data()),
        reinterpret_cast<const unsigned char*>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char*>(publicKey.data())
    ) == 0;
#else
    qWarning() << "SignatureService: Ed25519 verification requires libsodium";
    return false;
#endif
}

void SignatureService::addTrustedKey(const QByteArray& publicKey, 
                                     const QString& label, 
                                     bool isOfficial) {
    if (publicKey.size() != 32) {
        qWarning() << "SignatureService: Invalid public key size (expected 32 bytes)";
        return;
    }

    TrustedKey key;
    key.publicKey = publicKey;
    key.label = label;
    key.isOfficial = isOfficial;

    m_trustedKeys[publicKey] = key;
    saveTrustedKeys();
}

void SignatureService::removeTrustedKey(const QByteArray& publicKey) {
    if (m_trustedKeys.remove(publicKey) > 0) {
        saveTrustedKeys();
    }
}

TrustLevel SignatureService::getKeyTrustLevel(const QByteArray& publicKey) const {
    if (!m_trustedKeys.contains(publicKey)) {
        return TrustLevel::Unverified;
    }

    const TrustedKey& key = m_trustedKeys[publicKey];
    return key.isOfficial ? TrustLevel::Official : TrustLevel::Verified;
}

QByteArray SignatureService::computeCartridgeHash(const QString& cartridgePath) {
    // Compute hash of manifest + database
    // This should match the hash computed during cartridge signing
    
    QByteArray manifest = readManifest(cartridgePath);
    if (manifest.isEmpty()) {
        return QByteArray();
    }

    // Read database file
    QFile dbFile(cartridgePath);
    if (!dbFile.open(QIODevice::ReadOnly)) {
        qWarning() << "SignatureService: Failed to open cartridge database";
        return QByteArray();
    }

    QByteArray dbData = dbFile.readAll();
    dbFile.close();

    // Combine manifest + database and hash
    QByteArray combined = manifest + dbData;
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(combined);

    return hash.result();
}

QByteArray SignatureService::readManifest(const QString& cartridgePath) {
    // Open SQLite database and read manifest from metadata table
    // For now, return empty - this needs SQLite integration
    // TODO: Implement manifest reading from cartridge SQLite database
    
    qWarning() << "SignatureService: Manifest reading not yet implemented";
    return QByteArray();
}

QByteArray SignatureService::extractSignature(const QByteArray& manifest) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(manifest, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "SignatureService: Failed to parse manifest JSON:" << error.errorString();
        return QByteArray();
    }

    QJsonObject obj = doc.object();
    if (!obj.contains("signature")) {
        return QByteArray(); // Unsigned
    }

    QString sigStr = obj["signature"].toString();
    return QByteArray::fromBase64(sigStr.toUtf8());
}

QByteArray SignatureService::extractPublicKey(const QByteArray& manifest) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(manifest, &error);
    
    if (error.error != QJsonParseError::NoError) {
        return QByteArray();
    }

    QJsonObject obj = doc.object();
    if (!obj.contains("publicKey")) {
        return QByteArray();
    }

    QString keyStr = obj["publicKey"].toString();
    return QByteArray::fromBase64(keyStr.toUtf8());
}

void SignatureService::loadTrustedKeys() {
    m_trustedKeys.clear();

    int size = m_settings->beginReadArray("trustedKeys");
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        
        QByteArray publicKey = QByteArray::fromBase64(
            m_settings->value("publicKey").toByteArray()
        );
        QString label = m_settings->value("label").toString();
        bool isOfficial = m_settings->value("isOfficial", false).toBool();

        if (publicKey.size() == 32) {
            TrustedKey key;
            key.publicKey = publicKey;
            key.label = label;
            key.isOfficial = isOfficial;
            m_trustedKeys[publicKey] = key;
        }
    }
    m_settings->endArray();
}

void SignatureService::saveTrustedKeys() {
    m_settings->beginWriteArray("trustedKeys");
    int index = 0;
    for (auto it = m_trustedKeys.constBegin(); it != m_trustedKeys.constEnd(); ++it) {
        m_settings->setArrayIndex(index++);
        m_settings->setValue("publicKey", it.value().publicKey.toBase64());
        m_settings->setValue("label", it.value().label);
        m_settings->setValue("isOfficial", it.value().isOfficial);
    }
    m_settings->endArray();
    m_settings->sync();
}

} // namespace CodexiumMagnus::Services
