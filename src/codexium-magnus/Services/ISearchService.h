#ifndef ISEARCHSERVICE_H
#define ISEARCHSERVICE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>

namespace CodexiumMagnus::Services {

/**
 * Service interface for full-text search functionality.
 * 
 * Provides full-text search capabilities using SQLite FTS5 (FR-2, FR-3).
 * Supports boolean, phrase, fuzzy, and wildcard search modes with result
 * highlighting and in-page navigation.
 * 
 * The service performs searches on loaded cartridge content and returns
 * results with titles and highlighted snippets for display in the UI.
 */
class ISearchService : public QObject {
    Q_OBJECT

public:
    explicit ISearchService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ISearchService() = default;

    /**
     * Perform a full-text search on the loaded cartridge content.
     * 
     * Executes a search query using SQLite FTS5 with the specified options.
     * Results are returned asynchronously via the searchCompleted signal.
     * 
     * @param query The search query string
     * @param caseSensitive If true, perform case-sensitive search (default: false)
     * @param fuzzy If true, use fuzzy matching (via FTS5 NEAR operator) (default: false)
     * @param wildcards If true, enable wildcard matching (default: false)
     */
    virtual void performSearch(const QString& query, 
                              bool caseSensitive = false,
                              bool fuzzy = false,
                              bool wildcards = false) = 0;

signals:
    /**
     * Emitted when a search operation completes successfully.
     * 
     * The results are provided as a list of pairs, where each pair contains:
     * - First element: Document title
     * - Second element: Highlighted snippet with search terms marked
     * 
     * @param results List of (title, snippet) pairs representing search results
     */
    void searchCompleted(const QList<QPair<QString, QString>>& results);

    /**
     * Emitted when an error occurs during search operations.
     * 
     * Common errors include:
     * - No cartridge loaded
     * - FTS5 table not available
     * - Database connection issues
     * 
     * @param errorMessage Description of the error that occurred
     */
    void searchError(const QString& errorMessage);
};

} // namespace CodexiumMagnus::Services

#endif // ISEARCHSERVICE_H