#ifndef SEARCHSERVICE_H
#define SEARCHSERVICE_H

#include "ISearchService.h"
#include <QSqlDatabase>
#include <QString>

namespace CodexiumMagnus::Services {

class ICartridgeService;

/**
 * Implementation of ISearchService using SQLite FTS5.
 * 
 * Provides full-text search capabilities using SQLite's FTS5 extension.
 * Falls back to LIKE-based search if FTS5 tables are not available.
 * 
 * The service performs searches on loaded cartridge content and returns
 * results with titles and highlighted snippets. Supports boolean, phrase,
 * fuzzy, and wildcard search modes.
 */
class SearchService : public ISearchService {
    Q_OBJECT

public:
    /**
     * Construct a new SearchService instance.
     * 
     * @param cartridgeService The cartridge service to use for database access
     * @param parent Parent QObject for memory management
     */
    explicit SearchService(ICartridgeService *cartridgeService, QObject *parent = nullptr);
    
    /**
     * Destructor.
     */
    ~SearchService();

    /**
     * Perform a full-text search on the loaded cartridge content.
     * 
     * Executes a search query using SQLite FTS5 with the specified options.
     * Results are returned asynchronously via the searchCompleted signal.
     * 
     * @param query The search query string
     * @param caseSensitive If true, perform case-sensitive search (default: false)
     * @param fuzzy If true, use fuzzy matching via FTS5 NEAR operator (default: false)
     * @param wildcards If true, enable wildcard matching (default: false)
     */
    void performSearch(const QString& query,
                      bool caseSensitive = false,
                      bool fuzzy = false,
                      bool wildcards = false) override;

private:
    /**
     * Build an FTS5 query string from user input.
     * 
     * Escapes special characters and formats the query according to the
     * selected search mode (wildcards, fuzzy, phrase).
     * 
     * @param query The user's search query
     * @param caseSensitive Whether the search should be case-sensitive
     * @param fuzzy Whether to use fuzzy matching
     * @param wildcards Whether to enable wildcard matching
     * @return Formatted FTS5 query string
     */
    QString buildFtsQuery(const QString& query, 
                         bool caseSensitive, 
                         bool fuzzy, 
                         bool wildcards);

    /**
     * Perform a fallback search using LIKE when FTS5 is not available.
     * 
     * This method is called when the FTS5 table doesn't exist or is
     * unavailable. It performs a simple LIKE-based search on the documents
     * table as a fallback.
     * 
     * @param database The database connection to use
     * @param query The search query
     * @param caseSensitive Whether the search should be case-sensitive
     * @param results Output parameter for search results
     */
    void performFallbackSearch(QSqlDatabase& database, 
                               const QString& query,
                               bool caseSensitive,
                               QList<QPair<QString, QString>>& results);

    ICartridgeService *m_cartridgeService;  ///< Reference to cartridge service for database access
};

} // namespace CodexiumMagnus::Services

#endif // SEARCHSERVICE_H