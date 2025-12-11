#ifndef SEARCHSERVICETESTS_H
#define SEARCHSERVICETESTS_H

#include <QtTest/QtTest>
#include <QTemporaryFile>

class SearchServiceTests : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    
    // Search execution tests
    void performSearch_ValidQuery_EmitsResults();
    void performSearch_EmptyQuery_EmitsError();
    void performSearch_NoCartridge_EmitsError();
    void performSearch_MatchingQuery_ReturnsResults();
    void performSearch_NonMatchingQuery_ReturnsEmpty();
    
    // Search options tests
    void performSearch_CaseSensitive_RespectsCase();
    void performSearch_Fuzzy_AllowsVariations();
    void performSearch_Wildcards_AllowsWildcards();
    
    // Signal tests
    void performSearch_ValidQuery_EmitsSearchCompleted();
    void performSearch_InvalidQuery_EmitsSearchError();

private:
    void* m_service; // SearchService* - using void* to avoid include in header
    void* m_cartridgeService; // CartridgeService* - using void* to avoid include in header
    QTemporaryFile* m_testCartridge;
};

#endif // SEARCHSERVICETESTS_H
