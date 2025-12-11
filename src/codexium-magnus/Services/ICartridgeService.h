#ifndef ICARTRIDGESERVICE_H
#define ICARTRIDGESERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStandardItemModel>

namespace CodexiumMagnus::Services {

/**
 * Service interface for loading and managing cartridges.
 * 
 * Provides abstraction for cartridge loading, navigation, and content access.
 * Implementations handle SQLite cartridge files and expose navigation models
 * and document content to the UI layer.
 */
class ICartridgeService : public QObject {
    Q_OBJECT

public:
    explicit ICartridgeService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ICartridgeService() = default;

    /**
     * Load a cartridge from the specified file path.
     * @param path Path to the cartridge file (SQLite database)
     * @return true if cartridge loaded successfully, false otherwise
     */
    virtual bool loadCartridge(const QString& path) = 0;

    /**
     * Unload the currently loaded cartridge.
     * Cleans up resources and emits cartridgeUnloaded signal.
     */
    virtual void unloadCartridge() = 0;

    /**
     * Check if a cartridge is currently loaded.
     * @return true if a cartridge is loaded, false otherwise
     */
    virtual bool isCartridgeLoaded() const = 0;

    /**
     * Get the name of the currently loaded cartridge.
     * @return Cartridge name (typically the base filename without extension)
     */
    virtual QString getCartridgeName() const = 0;

    /**
     * Get the file path of the currently loaded cartridge.
     * @return Full path to the cartridge file, or empty string if none loaded
     */
    virtual QString getCartridgePath() const = 0;
    
    /**
     * Get the navigation model for the loaded cartridge.
     * The model represents the hierarchical structure (corpus → volume → document).
     * @return Pointer to QStandardItemModel, or nullptr if no cartridge loaded
     */
    virtual QStandardItemModel* getNavigationModel() = 0;

    /**
     * Get the HTML content for a specific document.
     * @param documentId Unique identifier for the document
     * @return HTML content string, or empty string if document not found
     */
    virtual QString getDocumentContent(const QString& documentId) = 0;

    /**
     * Get a list of all document IDs in the loaded cartridge.
     * @return List of document identifiers
     */
    virtual QStringList getDocumentList() const = 0;

signals:
    /**
     * Emitted when a cartridge is successfully loaded.
     * @param cartridgeName Name of the loaded cartridge
     */
    void cartridgeLoaded(const QString& cartridgeName);

    /**
     * Emitted when the current cartridge is unloaded.
     */
    void cartridgeUnloaded();

    /**
     * Emitted when an error occurs during cartridge operations.
     * @param errorMessage Description of the error
     */
    void errorOccurred(const QString& errorMessage);
};

} // namespace CodexiumMagnus::Services

#endif // ICARTRIDGESERVICE_H