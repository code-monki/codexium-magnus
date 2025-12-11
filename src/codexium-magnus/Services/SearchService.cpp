#include "SearchService.h"
#include "ICartridgeService.h"
#include "CartridgeService.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>
#include <QRegularExpression>

namespace CodexiumMagnus::Services {

SearchService::SearchService(ICartridgeService *cartridgeService, QObject *parent)
    : ISearchService(parent)
    , m_cartridgeService(cartridgeService)
{
}

SearchService::~SearchService() {
    // Qt parent system handles cleanup
}

void SearchService::performSearch(const QString& query,
                                 bool caseSensitive,
                                 bool fuzzy,
                                 bool wildcards) {
    // Validate input
    QString trimmedQuery = query.trimmed();
    if (trimmedQuery.isEmpty()) {
        emit searchError("Search query is empty");
        return;
    }

    if (!m_cartridgeService || !m_cartridgeService->isCartridgeLoaded()) {
        emit searchError("No cartridge loaded");
        return;
    }

    CartridgeService *cartridge = qobject_cast<CartridgeService*>(m_cartridgeService);
    if (!cartridge) {
        emit searchError("Invalid cartridge service");
        return;
    }

    QSqlDatabase *database = cartridge->getDatabase();
    if (!database || !database->isOpen()) {
        emit searchError("Database connection not available");
        return;
    }

    QList<QPair<QString, QString>> results;
    
    // Build FTS5 query
    QString ftsQuery = buildFtsQuery(trimmedQuery, caseSensitive, fuzzy, wildcards);
    
    if (ftsQuery.isEmpty()) {
        emit searchError("Invalid search query");
        return;
    }
    
    // Execute FTS5 search
    // Assuming FTS5 table named "content_fts" with columns: rowid, title, content, document_id
    // The actual schema may vary - adjust column names as needed
    QString sql = QString(R"(
        SELECT 
            snippet(content_fts, 2, '<mark>', '</mark>', '...', 32) as snippet,
            title,
            document_id
        FROM content_fts
        WHERE content_fts MATCH ?
        ORDER BY rank
        LIMIT 100
    )");
    
    QSqlQuery sqlQuery(*database);
    sqlQuery.prepare(sql);
    sqlQuery.addBindValue(ftsQuery);
    
    if (sqlQuery.exec()) {
        int resultCount = 0;
        while (sqlQuery.next()) {
            QString title = sqlQuery.value("title").toString();
            QString snippet = sqlQuery.value("snippet").toString();
            QString documentId = sqlQuery.value("document_id").toString();
            
            // Clean up snippet HTML if needed
            if (snippet.isEmpty()) {
                snippet = title; // Use title as fallback if no snippet
            }
            
            results.append(qMakePair(title, snippet));
            resultCount++;
        }
        
        qDebug() << "Search completed:" << resultCount << "results found for query:" << trimmedQuery;
        emit searchCompleted(results);
    } else {
        QString error = sqlQuery.lastError().text();
        qWarning() << "FTS5 search error:" << error;
        
        // If FTS5 table doesn't exist, try fallback search on regular tables
        if (error.contains("no such table", Qt::CaseInsensitive) || 
            error.contains("content_fts", Qt::CaseInsensitive)) {
            qDebug() << "FTS5 table not found, falling back to LIKE search";
            performFallbackSearch(*database, trimmedQuery, caseSensitive, results);
        } else {
            emit searchError(QString("Search failed: %1").arg(error));
        }
    }
}

void SearchService::performFallbackSearch(QSqlDatabase& database, 
                                         const QString& query,
                                         bool caseSensitive,
                                         QList<QPair<QString, QString>>& results) {
    // Fallback search using LIKE when FTS5 is not available
    QString sql = QString(R"(
        SELECT 
            id,
            title,
            substr(content, 1, 200) as snippet
        FROM documents
        WHERE title LIKE ? OR content LIKE ?
        LIMIT 100
    )");
    
    QString searchPattern = caseSensitive ? 
        QString("%%1%").arg(query) : 
        QString("%%1%").arg(query.toLower());
    
    QSqlQuery sqlQuery(database);
    sqlQuery.prepare(sql);
    sqlQuery.addBindValue(searchPattern);
    sqlQuery.addBindValue(searchPattern);
    
    if (sqlQuery.exec()) {
        int resultCount = 0;
        while (sqlQuery.next()) {
            QString title = sqlQuery.value("title").toString();
            QString snippet = sqlQuery.value("snippet").toString();
            
            if (snippet.isEmpty()) {
                snippet = title; // Use title as fallback
            }
            
            results.append(qMakePair(title, snippet));
            resultCount++;
        }
        
        qDebug() << "Fallback search completed:" << resultCount << "results found";
        emit searchCompleted(results);
    } else {
        QString error = sqlQuery.lastError().text();
        qWarning() << "Fallback search error:" << error;
        emit searchError(QString("Fallback search failed: %1").arg(error));
    }
}

QString SearchService::buildFtsQuery(const QString& query, 
                                     bool caseSensitive,
                                     bool fuzzy,
                                     bool wildcards) {
    QString ftsQuery = query.trimmed();
    
    if (ftsQuery.isEmpty()) {
        return QString();
    }
    
    // Escape special FTS5 characters: ", ', \, and control characters
    // FTS5 uses " for phrases and \ for escaping
    ftsQuery.replace("\\", "\\\\");
    ftsQuery.replace("\"", "\"\"");
    
    // Remove control characters that might cause issues
    ftsQuery.remove(QRegularExpression("[\\x00-\\x1F]"));
    
    if (wildcards) {
        // FTS5 supports * wildcard at end of terms
        // Convert space-separated terms to FTS5 format with wildcards
        QStringList terms = ftsQuery.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (terms.isEmpty()) {
            return QString();
        }
        
        for (QString& term : terms) {
            // Remove any existing wildcards first
            term.remove("*");
            // Add wildcard at end
            if (!term.isEmpty()) {
                term += "*";
            }
        }
        ftsQuery = terms.join(" ");
    } else if (fuzzy) {
        // FTS5 doesn't have built-in fuzzy search, but we can use NEAR operator
        // For true fuzzy, we'd need to implement Levenshtein distance
        // For now, use NEAR for multi-word queries to find terms near each other
        QStringList terms = ftsQuery.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (terms.isEmpty()) {
            return QString();
        }
        
        if (terms.size() > 1) {
            // Use NEAR for multi-word queries (terms within 10 words of each other)
            ftsQuery = terms.join(" NEAR/10 ");
        } else {
            // Single word - use as-is (FTS5 will find similar)
            ftsQuery = terms.first();
        }
    } else {
        // Phrase search for exact matches
        ftsQuery = "\"" + ftsQuery + "\"";
    }
    
    // Case sensitivity: FTS5 is case-insensitive by default
    // If case-sensitive is required, we'd need to use a case-sensitive FTS5 table
    // For now, we'll rely on FTS5's default case-insensitive behavior
    // Note: The caseSensitive parameter is kept for API compatibility but
    // FTS5 case sensitivity is determined at table creation time
    
    return ftsQuery;
}

} // namespace CodexiumMagnus::Services