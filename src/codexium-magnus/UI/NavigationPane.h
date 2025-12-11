#ifndef NAVIGATIONPANE_H
#define NAVIGATIONPANE_H

#include <QWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QLabel>
#include <QStandardItemModel>

namespace CodexiumMagnus::UI {

/**
 * Navigation pane showing corpus → volume → document hierarchy.
 * Populated by Cartridge Service.
 * Uses QTreeView with custom model.
 */
class NavigationPane : public QWidget {
    Q_OBJECT

public:
    explicit NavigationPane(QWidget *parent = nullptr);
    ~NavigationPane();

    void setNavigationModel(QStandardItemModel *model);
    void clear();

signals:
    void documentSelected(const QString& documentId);
    void sectionSelected(const QString& sectionId);

private slots:
    void onItemClicked(const QModelIndex& index);

private:
    QVBoxLayout *m_layout;
    QLabel *m_titleLabel;
    QTreeView *m_treeView;
    QStandardItemModel *m_model;
};

} // namespace CodexiumMagnus::UI

#endif // NAVIGATIONPANE_H