#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include "../codexium-magnus-core/Models/TypographyConfig.h"
#include "../codexium-magnus-core/Models/BibliographyConfig.h"

namespace CodexiumMagnus::UI {

class TypographySettingsWidget;
class BibliographySettingsWidget;

/**
 * Settings dialog for user preferences.
 * 
 * Provides UI for adjusting Typography (FR-11) and Bibliography (FR-12) settings.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    /**
     * Get the current typography configuration from the dialog.
     * @return TypographyConfig with user's settings
     */
    Core::Models::TypographyConfig getTypographyConfig() const;

    /**
     * Get the current bibliography configuration from the dialog.
     * @return BibliographyConfig with user's settings
     */
    Core::Models::BibliographyConfig getBibliographyConfig() const;

    /**
     * Set the typography configuration to display in the dialog.
     * @param config TypographyConfig to display
     */
    void setTypographyConfig(const Core::Models::TypographyConfig& config);

    /**
     * Set the bibliography configuration to display in the dialog.
     * @param config BibliographyConfig to display
     */
    void setBibliographyConfig(const Core::Models::BibliographyConfig& config);

signals:
    /**
     * Emitted when settings are accepted.
     * @param typography Typography configuration
     * @param bibliography Bibliography configuration
     */
    void settingsAccepted(const Core::Models::TypographyConfig& typography,
                         const Core::Models::BibliographyConfig& bibliography);

public slots:
    /**
     * Public slot to trigger settings save (for testing).
     */
    void saveSettingsNow();

private slots:
    void onAccepted();
    void onRejected();

private:
    void setupUi();
    void loadSettings();
    void saveSettings();

    QTabWidget *m_tabWidget;
    TypographySettingsWidget *m_typographyWidget;
    BibliographySettingsWidget *m_bibliographyWidget;
    QDialogButtonBox *m_buttonBox;
};

} // namespace CodexiumMagnus::UI

#endif // SETTINGSDIALOG_H

