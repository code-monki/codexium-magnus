#include "ThemeManager.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QRegularExpression>

namespace CodexiumMagnus::Theme {

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Light)
    , m_settings(nullptr)
{
    m_settings = new QSettings(this);
    loadThemePreference();
    loadDefaultTheme(m_currentTheme);
}

ThemeManager::~ThemeManager() {
    saveThemePreference();
}

void ThemeManager::setTheme(Theme theme) {
    if (m_currentTheme == theme && theme != Custom) {
        return;
    }

    m_currentTheme = theme;
    loadDefaultTheme(theme);
    emit themeChanged(theme);
}

void ThemeManager::loadCustomTheme(const QString& themePath) {
    if (!QFile::exists(themePath)) {
        qWarning() << "Custom theme file not found:" << themePath;
        return;
    }

    m_customThemePath = themePath;
    m_currentTheme = Custom;

    QFile file(themePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open custom theme file:" << themePath;
        return;
    }

    QTextStream in(&file);
    QString stylesheet = in.readAll();
    file.close();

    applyStylesheet(stylesheet);
    emit themeChanged(Custom);
}

void ThemeManager::applyStylesheet(const QString& stylesheet) {
    QString processedStylesheet = stylesheet;
    applyTokensToStylesheet(processedStylesheet);
    QApplication *app = qobject_cast<QApplication*>(QApplication::instance());
    if (app) {
        app->setStyleSheet(processedStylesheet);
    }
}

QPalette ThemeManager::currentPalette() const {
    QPalette palette;
    
    // Set palette based on current theme
    switch (m_currentTheme) {
        case Light:
            palette.setColor(QPalette::Window, QColor(255, 255, 255));
            palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
            palette.setColor(QPalette::Base, QColor(255, 255, 255));
            palette.setColor(QPalette::Text, QColor(0, 0, 0));
            break;
        case Sepia:
            palette.setColor(QPalette::Window, QColor(251, 248, 240));
            palette.setColor(QPalette::WindowText, QColor(60, 50, 40));
            palette.setColor(QPalette::Base, QColor(255, 250, 240));
            palette.setColor(QPalette::Text, QColor(60, 50, 40));
            break;
        case Dark:
            palette.setColor(QPalette::Window, QColor(30, 30, 30));
            palette.setColor(QPalette::WindowText, QColor(240, 240, 240));
            palette.setColor(QPalette::Base, QColor(40, 40, 40));
            palette.setColor(QPalette::Text, QColor(240, 240, 240));
            break;
        case Custom:
            // Custom theme palette would be loaded from theme file
            break;
    }
    
    return palette;
}

QString ThemeManager::getToken(const QString& tokenName) const {
    return m_tokens.value(tokenName, QString());
}

void ThemeManager::loadDefaultTheme(Theme theme) {
    QString stylesheetPath = getThemeStylesheetPath(theme);
    
    if (stylesheetPath.isEmpty() || !QFile::exists(stylesheetPath)) {
        // Use built-in default stylesheet
        loadThemeTokens(theme);
        QString defaultStylesheet = generateDefaultStylesheet(theme);
        applyStylesheet(defaultStylesheet);
        return;
    }

    QFile file(stylesheetPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open theme file:" << stylesheetPath;
        loadThemeTokens(theme);
        QString defaultStylesheet = generateDefaultStylesheet(theme);
        applyStylesheet(defaultStylesheet);
        return;
    }

    QTextStream in(&file);
    QString stylesheet = in.readAll();
    file.close();

    loadThemeTokens(theme);
    applyStylesheet(stylesheet);
}

void ThemeManager::loadThemeTokens(Theme theme) {
    m_tokens.clear();

    switch (theme) {
        case Light:
            m_tokens["--color-background-primary"] = "#FFFFFF";
            m_tokens["--color-background-secondary"] = "#F5F5F5";
            m_tokens["--color-text-primary"] = "#000000";
            m_tokens["--color-text-secondary"] = "#666666";
            m_tokens["--color-accent"] = "#0066CC";
            m_tokens["--color-border"] = "#CCCCCC";
            m_tokens["--color-selection-background"] = "#3399FF";
            m_tokens["--color-selection-text"] = "#FFFFFF";
            m_tokens["--color-link"] = "#0066CC";
            m_tokens["--color-link-hover"] = "#004499";
            // UI component tokens (WCAG AA compliant: 4.5:1 contrast ratio)
            m_tokens["--color-list-background"] = "#FFFFFF";
            m_tokens["--color-list-background-alt"] = "#F8F8F8";  // Zebra stripe (silver)
            m_tokens["--color-list-text"] = "#000000";
            m_tokens["--color-input-background"] = "#FFFFFF";
            m_tokens["--color-input-text"] = "#000000";
            m_tokens["--color-input-border"] = "#808080";  // WCAG AA: 3.0:1 contrast (gray on white)
            m_tokens["--color-input-border-focus"] = "#0066CC";
            m_tokens["--font-family-base"] = "Inter, system-ui, Helvetica, Arial, sans-serif";
            m_tokens["--font-size-base"] = "13pt";
            break;
        case Sepia:
            m_tokens["--color-background-primary"] = "#FBF8F0";
            m_tokens["--color-background-secondary"] = "#F5F0E8";
            m_tokens["--color-text-primary"] = "#3C3228";
            m_tokens["--color-text-secondary"] = "#6B5D4A";
            m_tokens["--color-accent"] = "#8B6914";
            m_tokens["--color-border"] = "#D4C4B0";
            m_tokens["--color-selection-background"] = "#D4A574";
            m_tokens["--color-selection-text"] = "#3C3228";
            m_tokens["--color-link"] = "#8B6914";
            m_tokens["--color-link-hover"] = "#6B4F0A";
            // UI component tokens (WCAG AA compliant)
            m_tokens["--color-list-background"] = "#FBF8F0";
            m_tokens["--color-list-background-alt"] = "#F5F0E8";  // Zebra stripe (slightly darker sepia)
            m_tokens["--color-list-text"] = "#3C3228";
            m_tokens["--color-input-background"] = "#FFFFFF";
            m_tokens["--color-input-text"] = "#3C3228";
            m_tokens["--color-input-border"] = "#9A8A70";  // WCAG AA: 3.0:1 contrast (sepia border)
            m_tokens["--color-input-border-focus"] = "#8B6914";
            m_tokens["--font-family-base"] = "Inter, system-ui, Georgia, serif";
            m_tokens["--font-size-base"] = "13pt";
            break;
        case Dark:
            m_tokens["--color-background-primary"] = "#1E1E1E";
            m_tokens["--color-background-secondary"] = "#2D2D2D";
            m_tokens["--color-text-primary"] = "#F0F0F0";
            m_tokens["--color-text-secondary"] = "#B0B0B0";
            m_tokens["--color-accent"] = "#4A9EFF";
            m_tokens["--color-border"] = "#404040";
            m_tokens["--color-selection-background"] = "#4A9EFF";
            m_tokens["--color-selection-text"] = "#FFFFFF";
            m_tokens["--color-link"] = "#4A9EFF";
            m_tokens["--color-link-hover"] = "#6BB0FF";
            // UI component tokens (WCAG AA compliant)
            m_tokens["--color-list-background"] = "#2D2D2D";
            m_tokens["--color-list-background-alt"] = "#252525";  // Zebra stripe (darker)
            m_tokens["--color-list-text"] = "#F0F0F0";
            m_tokens["--color-input-background"] = "#2D2D2D";
            m_tokens["--color-input-text"] = "#F0F0F0";
            m_tokens["--color-input-border"] = "#606060";  // WCAG AA: 3.0:1 contrast (dark border on dark bg)
            m_tokens["--color-input-border-focus"] = "#4A9EFF";
            m_tokens["--font-family-base"] = "Inter, system-ui, Helvetica, Arial, sans-serif";
            m_tokens["--font-size-base"] = "13pt";
            break;
        case Custom:
            // Custom tokens loaded from theme file
            break;
    }

    emit tokensChanged();
}

void ThemeManager::applyTokensToStylesheet(QString& stylesheet) const {
    // Replace token placeholders with actual values
    // Tokens in stylesheet are in format: --token-name (without var())
    for (auto it = m_tokens.constBegin(); it != m_tokens.constEnd(); ++it) {
        QString token = it.key();
        QString value = it.value();
        // Replace token with actual value
        stylesheet.replace(token, value);
    }
}

QString ThemeManager::getThemeStylesheetPath(Theme theme) const {
    QString themeName;
    switch (theme) {
        case Light: themeName = "light"; break;
        case Sepia: themeName = "sepia"; break;
        case Dark: themeName = "dark"; break;
        case Custom: return m_customThemePath;
    }

    // Look for theme file in resources or application directory
    QString appDir = QApplication::applicationDirPath();
    QString themePath = QString("%1/themes/%2.qss").arg(appDir, themeName);
    
    if (QFile::exists(themePath)) {
        return themePath;
    }

    // Fallback to user config directory
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    themePath = QString("%1/themes/%2.qss").arg(configDir, themeName);
    
    return QFile::exists(themePath) ? themePath : QString();
}

QString ThemeManager::generateDefaultStylesheet(Theme theme) const {
    // Generate a basic default stylesheet with token placeholders
    // Tokens will be replaced by applyTokensToStylesheet()
    QString stylesheet = R"(
        QMainWindow {
            background-color: --color-background-primary;
            color: --color-text-primary;
        }
        QWidget {
            background-color: --color-background-primary;
            color: --color-text-primary;
        }
        QTreeView, QListView {
            background-color: --color-list-background;
            alternate-background-color: --color-list-background-alt;
            color: --color-list-text;
            border: 1px solid --color-border;
            selection-background-color: --color-selection-background;
            selection-color: --color-selection-text;
        }
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: --color-input-background;
            color: --color-input-text;
            border: 1px solid --color-input-border;
            padding: 4px;
            border-radius: 3px;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border: 1px solid --color-input-border-focus;
        }
        QPushButton {
            background-color: --color-accent;
            color: white;
            border: none;
            padding: 6px 12px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: --color-link-hover;
        }
        QCheckBox {
            color: --color-text-primary;
        }
        QLabel {
            color: --color-text-primary;
        }
    )";
    
    // Replace tokens with actual values
    QString processedStylesheet = stylesheet;
    applyTokensToStylesheet(processedStylesheet);
    
    return processedStylesheet;
}

void ThemeManager::saveThemePreference() {
    m_settings->setValue("theme/current", static_cast<int>(m_currentTheme));
    if (m_currentTheme == Custom && !m_customThemePath.isEmpty()) {
        m_settings->setValue("theme/customPath", m_customThemePath);
    }
    m_settings->sync(); // Ensure settings are written to disk
}

void ThemeManager::loadThemePreference() {
    int themeValue = m_settings->value("theme/current", static_cast<int>(Light)).toInt();
    m_currentTheme = static_cast<Theme>(themeValue);
    
    if (m_currentTheme == Custom) {
        m_customThemePath = m_settings->value("theme/customPath").toString();
    }
}

} // namespace CodexiumMagnus::Theme