#ifndef SEARCHPANE_H
#define SEARCHPANE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QListView>
#include <QStandardItemModel>
#include <QComboBox>

namespace CodexiumMagnus::UI {

/**
 * Search pane with textbox, filters (fuzzy, case, wildcard).
 * Binds to Search Service.
 * Displays results with snippet/heading (QListView).
 */
class SearchPane : public QWidget {
    Q_OBJECT

public:
    explicit SearchPane(QWidget *parent = nullptr);
    ~SearchPane();

    QString searchQuery() const;
    bool isCaseSensitive() const;
    bool isFuzzy() const;
    bool useWildcards() const;

    void setResults(const QList<QPair<QString, QString>>& results); // (title, snippet) pairs
    void clearResults();

signals:
    void searchRequested(const QString& query);
    void resultSelected(const QString& resultId);

private slots:
    void onSearchButtonClicked();
    void onSearchTextChanged();
    void onResultClicked(const QModelIndex& index);

private:
    void setupUi();

    QVBoxLayout *m_mainLayout;
    QLabel *m_titleLabel;
    QLineEdit *m_searchEdit;
    QPushButton *m_searchButton;
    QHBoxLayout *m_filterLayout;
    QCheckBox *m_caseSensitiveCheck;
    QCheckBox *m_fuzzyCheck;
    QCheckBox *m_wildcardCheck;
    QListView *m_resultsList;
    QStandardItemModel *m_resultsModel;
};

} // namespace CodexiumMagnus::UI

#endif // SEARCHPANE_H