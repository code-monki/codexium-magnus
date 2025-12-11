#include "NavigationPane.h"
#include <QHeaderView>

namespace CodexiumMagnus::UI {

NavigationPane::NavigationPane(QWidget *parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_titleLabel(nullptr)
    , m_treeView(nullptr)
    , m_model(nullptr)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setSpacing(4);

    m_titleLabel = new QLabel("Navigation", this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    m_layout->addWidget(m_titleLabel);

    m_treeView = new QTreeView(this);
    m_treeView->setHeaderHidden(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setAnimated(true);
    
    m_model = new QStandardItemModel(this);
    m_treeView->setModel(m_model);
    
    connect(m_treeView, &QTreeView::clicked, this, &NavigationPane::onItemClicked);
    
    m_layout->addWidget(m_treeView);
}

NavigationPane::~NavigationPane() {
    // Qt parent system handles cleanup
}

void NavigationPane::setNavigationModel(QStandardItemModel *model) {
    if (m_model && m_model != model) {
        m_treeView->setModel(nullptr);
        if (m_model->parent() == this) {
            delete m_model;
        }
    }
    
    m_model = model;
    if (m_model) {
        m_treeView->setModel(m_model);
    }
}

void NavigationPane::clear() {
    if (m_model) {
        m_model->clear();
    }
}

void NavigationPane::onItemClicked(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }
    
    QVariant data = index.data(Qt::UserRole);
    if (data.isValid()) {
        QString type = data.toString();
        QString id = index.data(Qt::UserRole + 1).toString();
        
        if (type == "document") {
            emit documentSelected(id);
        } else if (type == "section") {
            emit sectionSelected(id);
        }
    }
}

} // namespace CodexiumMagnus::UI