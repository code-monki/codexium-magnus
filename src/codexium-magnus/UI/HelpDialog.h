#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QVBoxLayout>

namespace CodexiumMagnus::UI {

/**
 * Help dialog displaying getting started guide, features, and keyboard shortcuts.
 * 
 * Provides accessible help content separate from the main content viewing area,
 * improving WCAG compliance by keeping the main content area focused on documents.
 */
class HelpDialog : public QDialog {
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

private:
    void setupUi();
    QString generateHelpContent() const;
    
    QVBoxLayout *m_layout;
    QTextBrowser *m_textBrowser;
    QDialogButtonBox *m_buttonBox;
};

} // namespace CodexiumMagnus::UI

#endif // HELPDIALOG_H
