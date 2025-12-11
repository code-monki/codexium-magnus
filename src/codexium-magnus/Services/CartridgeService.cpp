#include "CartridgeService.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QStandardItem>
#include <QMap>
#include <QDebug>

namespace CodexiumMagnus::Services {

CartridgeService::CartridgeService(QObject *parent)
    : ICartridgeService(parent)
    , m_cartridgePath()
    , m_cartridgeName()
    , m_navigationModel(nullptr)
    , m_isLoaded(false)
    , m_trustLevel(TrustLevel::Unverified)
    , m_signatureService(nullptr)
{
    m_navigationModel = new QStandardItemModel(this);
    // SignatureService will be set by MainWindow or created here if needed
}

CartridgeService::~CartridgeService() {
    unloadCartridge();
}

bool CartridgeService::loadCartridge(const QString& path) {
    if (m_isLoaded) {
        unloadCartridge();
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        emit errorOccurred(QString("Cartridge file not found or not readable: %1").arg(path));
        return false;
    }

    // Open SQLite database
    QString connectionName = QString("cartridge_%1").arg(reinterpret_cast<quintptr>(this));
    m_database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_database.setDatabaseName(path);

    if (!m_database.open()) {
        QString error = m_database.lastError().text();
        emit errorOccurred(QString("Failed to open cartridge: %1").arg(error));
        return false;
    }

    // Verify it's a valid cartridge (check for expected tables)
    QSqlQuery query(m_database);
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table'")) {
        emit errorOccurred("Failed to query cartridge structure");
        m_database.close();
        return false;
    }

    // Verify cartridge signature if signature service is available
    if (m_signatureService) {
        m_trustLevel = m_signatureService->verifyCartridge(path);
        emit trustLevelDetermined(m_trustLevel);
    } else {
        // No signature service available - mark as unverified
        m_trustLevel = TrustLevel::Unverified;
        emit trustLevelDetermined(m_trustLevel);
    }

    m_cartridgePath = path;
    m_cartridgeName = fileInfo.baseName();
    m_isLoaded = true;

    buildNavigationModel();
    emit cartridgeLoaded(m_cartridgeName);

    return true;
}

void CartridgeService::unloadCartridge() {
    if (m_isLoaded) {
        m_database.close();
        QString connectionName = m_database.connectionName();
        QSqlDatabase::removeDatabase(connectionName);
        
        m_cartridgePath.clear();
        m_cartridgeName.clear();
        m_isLoaded = false;
        
        if (m_navigationModel) {
            m_navigationModel->clear();
        }
        
        emit cartridgeUnloaded();
    }
}

bool CartridgeService::isCartridgeLoaded() const {
    return m_isLoaded;
}

QString CartridgeService::getCartridgeName() const {
    return m_cartridgeName;
}

QString CartridgeService::getCartridgePath() const {
    return m_cartridgePath;
}

QSqlDatabase* CartridgeService::getDatabase() const {
    if (!m_isLoaded) {
        return nullptr;
    }
    // Return non-const pointer to allow query execution
    // Note: This is safe because we control the database lifecycle
    return const_cast<QSqlDatabase*>(&m_database);
}

QStandardItemModel* CartridgeService::getNavigationModel() {
    return m_navigationModel;
}

QString CartridgeService::getDocumentContent(const QString& documentId) {
    if (!m_isLoaded) {
        return QString();
    }

    return queryDocumentContent(documentId);
}

QStringList CartridgeService::getDocumentList() const {
    QStringList documents;
    
    if (!m_isLoaded) {
        return documents;
    }

    QSqlQuery query(m_database);
    // TODO: Adjust query based on actual cartridge schema
    if (query.exec("SELECT id, title FROM documents ORDER BY title")) {
        while (query.next()) {
            documents.append(query.value(0).toString());
        }
    }

    return documents;
}

void CartridgeService::buildNavigationModel() {
    if (!m_isLoaded || !m_navigationModel) {
        return;
    }

    m_navigationModel->clear();

    // TODO: Build navigation tree based on actual cartridge schema
    // For now, create a placeholder structure
    QSqlQuery query(m_database);
    
    // Example: Query for corpus/volume/document structure
    // This is a placeholder - adjust based on actual schema
    if (query.exec("SELECT id, title, parent_id, type FROM navigation ORDER BY sort_order")) {
        QMap<QString, QStandardItem*> itemMap;
        
        while (query.next()) {
            QString id = query.value(0).toString();
            QString title = query.value(1).toString();
            QString parentId = query.value(2).toString();
            QString type = query.value(3).toString();
            
            QStandardItem *item = new QStandardItem(title);
            item->setData(type, Qt::UserRole);
            item->setData(id, Qt::UserRole + 1);
            
            if (parentId.isEmpty()) {
                m_navigationModel->appendRow(item);
            } else {
                QStandardItem *parent = itemMap.value(parentId);
                if (parent) {
                    parent->appendRow(item);
                } else {
                    m_navigationModel->appendRow(item);
                }
            }
            
            itemMap[id] = item;
        }
    } else {
        // Fallback: Create a simple placeholder
        QStandardItem *root = new QStandardItem(m_cartridgeName);
        root->setData("corpus", Qt::UserRole);
        m_navigationModel->appendRow(root);
    }
}

QString CartridgeService::queryDocumentContent(const QString& documentId) {
    if (!m_isLoaded) {
        return QString();
    }

    QSqlQuery query(m_database);
    // TODO: Adjust query based on actual cartridge schema
    query.prepare("SELECT content FROM documents WHERE id = ?");
    query.addBindValue(documentId);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }

    return QString();
}

} // namespace CodexiumMagnus::Services