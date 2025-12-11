#include "HelpDialog.h"
#include <QApplication>
#include <QKeySequence>

namespace CodexiumMagnus::UI {

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
    , m_layout(nullptr)
    , m_textBrowser(nullptr)
    , m_buttonBox(nullptr)
{
    setupUi();
    setWindowTitle("Help - Codexium Magnus");
    resize(700, 600);
}

HelpDialog::~HelpDialog() {
    // Qt parent-child relationship handles cleanup
}

void HelpDialog::setupUi() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setOpenExternalLinks(true);
    m_textBrowser->setHtml(generateHelpContent());
    m_layout->addWidget(m_textBrowser);
    
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    m_layout->addWidget(m_buttonBox);
    
    // Set focus to the text browser for keyboard navigation
    m_textBrowser->setFocus();
}

QString HelpDialog::generateHelpContent() const {
    QString openShortcut = QKeySequence(QKeySequence::Open).toString(QKeySequence::NativeText);
    QString printShortcut = QKeySequence(QKeySequence::Print).toString(QKeySequence::NativeText);
    QString zoomInShortcut = QKeySequence(QKeySequence::ZoomIn).toString(QKeySequence::NativeText);
    QString zoomOutShortcut = QKeySequence(QKeySequence::ZoomOut).toString(QKeySequence::NativeText);
    QString resetZoomShortcut = "Ctrl+0";
    QString prefsShortcut = QKeySequence(QKeySequence::Preferences).toString(QKeySequence::NativeText);
    
    return QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <style>
                body {
                    font-family: system-ui, -apple-system, 'Segoe UI', sans-serif;
                    padding: 20px;
                    line-height: 1.6;
                    max-width: 900px;
                    margin: 0 auto;
                }
                h1 {
                    font-size: 2em;
                    margin-bottom: 0.5em;
                    border-bottom: 2px solid currentColor;
                    padding-bottom: 0.3em;
                }
                h2 {
                    font-size: 1.5em;
                    margin-top: 1.5em;
                    margin-bottom: 0.5em;
                }
                p {
                    margin-bottom: 1em;
                }
                ul, ol {
                    margin-left: 2em;
                    margin-bottom: 1em;
                }
                li {
                    margin-bottom: 0.5em;
                }
                code {
                    background: rgba(128, 128, 128, 0.2);
                    padding: 2px 6px;
                    border-radius: 3px;
                    font-family: 'Monaco', 'Menlo', 'Courier New', monospace;
                    font-size: 0.9em;
                }
                .keyboard-shortcut {
                    display: inline-block;
                    background: rgba(128, 128, 128, 0.15);
                    padding: 2px 8px;
                    border-radius: 4px;
                    font-family: monospace;
                    font-size: 0.85em;
                    border: 1px solid rgba(128, 128, 128, 0.3);
                }
                .tip {
                    background: rgba(100, 150, 255, 0.1);
                    border-left: 4px solid rgba(100, 150, 255, 0.5);
                    padding: 12px 16px;
                    margin: 16px 0;
                    border-radius: 4px;
                }
            </style>
        </head>
        <body>
            <h1>Codexium Magnus Help</h1>
            
            <h2>Getting Started</h2>
            <p>To begin reading content:</p>
            <ol>
                <li>Go to <strong>File → Open Cartridge...</strong> (or press <span class="keyboard-shortcut">%1</span>) to load a cartridge database</li>
                <li>Browse documents using the navigation pane on the left</li>
                <li>Search for content using the search pane</li>
                <li>Click on any document or search result to view its content</li>
            </ol>
            
            <h2>Features</h2>
            <ul>
                <li><strong>Zoom Control:</strong> Use <span class="keyboard-shortcut">%2</span> to zoom in, <span class="keyboard-shortcut">%3</span> to zoom out, or <span class="keyboard-shortcut">%4</span> to reset to 100%%. Your zoom preference is saved automatically.</li>
                <li><strong>Themes:</strong> Switch between Light, Sepia, and Dark themes via the Theme menu. Themes apply token-based styling for consistent appearance.</li>
                <li><strong>Settings:</strong> Customize typography (fonts, sizes, heading scales) and bibliography preferences (citation styles, sorting, grouping) via Settings → Preferences (<span class="keyboard-shortcut">%5</span>).</li>
                <li><strong>Print & Export:</strong> Print documents to a physical printer (<span class="keyboard-shortcut">%6</span>) or export to PDF format.</li>
                <li><strong>Search:</strong> Full-text search with FTS5 support, including case-sensitive, fuzzy, and wildcard options.</li>
                <li><strong>Security:</strong> Ed25519 signature verification for cartridge authenticity and trust levels.</li>
            </ul>
            
            <div class="tip">
                <strong>💡 Tip:</strong> Your zoom level and theme preferences are automatically saved and will persist across sessions.
            </div>
            
            <h2>Keyboard Shortcuts</h2>
            <ul>
                <li><span class="keyboard-shortcut">%1</span> - Open Cartridge</li>
                <li><span class="keyboard-shortcut">%6</span> - Print</li>
                <li><span class="keyboard-shortcut">%2</span> - Zoom In</li>
                <li><span class="keyboard-shortcut">%3</span> - Zoom Out</li>
                <li><span class="keyboard-shortcut">%4</span> - Reset Zoom</li>
                <li><span class="keyboard-shortcut">%5</span> - Preferences</li>
            </ul>
        </body>
        </html>
    )").arg(openShortcut, zoomInShortcut, zoomOutShortcut, resetZoomShortcut, prefsShortcut, printShortcut);
}

} // namespace CodexiumMagnus::UI
