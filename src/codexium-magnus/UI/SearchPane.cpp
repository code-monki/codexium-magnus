#include "SearchPane.h"
#include <QKeyEvent>

namespace CodexiumMagnus::UI {

SearchPane::SearchPane(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_searchEdit(nullptr)
    , m_searchButton(nullptr)
    , m_filterLayout(nullptr)
    , m_caseSensitiveCheck(nullptr)
    , m_fuzzyCheck(nullptr)
    , m_wildcardCheck(nullptr)
    , m_resultsList(nullptr)
    , m_resultsModel(nullptr)
{
    setupUi();
}

SearchPane::~SearchPane() {
    // Qt parent system handles cleanup
}

void SearchPane::setupUi() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(4);

    m_titleLabel = new QLabel("Search", this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    m_mainLayout->addWidget(m_titleLabel);

    // Search input
    QHBoxLayout *searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Enter search query...");
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &SearchPane::onSearchButtonClicked);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SearchPane::onSearchTextChanged);
    
    m_searchButton = new QPushButton("Search", this);
    connect(m_searchButton, &QPushButton::clicked, this, &SearchPane::onSearchButtonClicked);
    
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    m_mainLayout->addLayout(searchLayout);

    // Filters
    m_filterLayout = new QHBoxLayout();
    m_caseSensitiveCheck = new QCheckBox("Case Sensitive", this);
    m_fuzzyCheck = new QCheckBox("Fuzzy", this);
    m_wildcardCheck = new QCheckBox("Wildcards", this);
    
    m_filterLayout->addWidget(m_caseSensitiveCheck);
    m_filterLayout->addWidget(m_fuzzyCheck);
    m_filterLayout->addWidget(m_wildcardCheck);
    m_mainLayout->addLayout(m_filterLayout);

    // Results list
    m_resultsList = new QListView(this);
    m_resultsModel = new QStandardItemModel(this);
    m_resultsList->setModel(m_resultsModel);
    connect(m_resultsList, &QListView::clicked, this, &SearchPane::onResultClicked);
    
    m_mainLayout->addWidget(m_resultsList);
}

QString SearchPane::searchQuery() const {
    return m_searchEdit->text();
}

bool SearchPane::isCaseSensitive() const {
    return m_caseSensitiveCheck->isChecked();
}

bool SearchPane::isFuzzy() const {
    return m_fuzzyCheck->isChecked();
}

bool SearchPane::useWildcards() const {
    return m_wildcardCheck->isChecked();
}

void SearchPane::setResults(const QList<QPair<QString, QString>>& results) {
    m_resultsModel->clear();
    
    for (const auto& result : results) {
        QStandardItem *item = new QStandardItem();
        item->setText(QString("%1\n%2").arg(result.first, result.second));
        item->setData(result.first, Qt::UserRole); // Store result ID in UserRole
        item->setToolTip(result.second);
        m_resultsModel->appendRow(item);
    }
}

void SearchPane::clearResults() {
    m_resultsModel->clear();
}

void SearchPane::onSearchButtonClicked() {
    QString query = m_searchEdit->text().trimmed();
    if (!query.isEmpty()) {
        emit searchRequested(query);
    }
}

void SearchPane::onSearchTextChanged() {
    // Could implement live search here if desired
}

void SearchPane::onResultClicked(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }
    
    QString resultId = index.data(Qt::UserRole).toString();
    if (!resultId.isEmpty()) {
        emit resultSelected(resultId);
    }
}

} // namespace CodexiumMagnus::UI