#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QPalette>
#include <QSettings>

namespace CodexiumMagnus::Theme {

/**
 * Manages theme switching, stylesheet loading, and token-based theming system.
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme {
        Light = 0,
        Sepia = 1,
        Dark = 2,
        Custom = 3
    };
    Q_ENUM(Theme)

    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    void setTheme(Theme theme);
    Theme currentTheme() const { return m_currentTheme; }
    
    void loadCustomTheme(const QString& themePath);
    QString customThemePath() const { return m_customThemePath; }
    
    void applyStylesheet(const QString& stylesheet);
    QPalette currentPalette() const;
    
    QMap<QString, QString> getTokenMap() const { return m_tokens; }
    QString getToken(const QString& tokenName) const;
    
    void saveThemePreference();
    void loadThemePreference();

signals:
    void themeChanged(Theme theme);
    void tokensChanged();

private:
    void loadDefaultTheme(Theme theme);
    void loadThemeTokens(Theme theme);
    void applyTokensToStylesheet(QString& stylesheet);
    QString getThemeStylesheetPath(Theme theme) const;
    QString generateDefaultStylesheet(Theme theme) const;

    Theme m_currentTheme;
    QString m_customThemePath;
    QMap<QString, QString> m_tokens;
    QSettings *m_settings;
};

} // namespace CodexiumMagnus::Theme

#endif // THEMEMANAGER_H