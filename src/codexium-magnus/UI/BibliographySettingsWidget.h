#ifndef BIBLIOGRAPHYSETTINGSWIDGET_H
#define BIBLIOGRAPHYSETTINGSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include "../../codexium-magnus-core/Models/BibliographyConfig.h"

namespace CodexiumMagnus::UI {

/**
 * Widget for configuring bibliography settings (FR-12).
 * 
 * Allows users to adjust:
 * - Citation style (APA, CMS)
 * - Sort order (author, year, title)
 * - Grouping (by year, by author, none)
 */
class BibliographySettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit BibliographySettingsWidget(QWidget *parent = nullptr);
    ~BibliographySettingsWidget();

    /**
     * Get the current bibliography configuration.
     * @return BibliographyConfig with current widget values
     */
    Core::Models::BibliographyConfig getConfig() const;

    /**
     * Set the bibliography configuration to display.
     * @param config BibliographyConfig to display
     */
    void setConfig(const Core::Models::BibliographyConfig& config);

private slots:
    void onStyleChanged(const QString& style);
    void onSortByChanged(const QString& sortBy);
    void onGroupByChanged(const QString& groupBy);

private:
    void setupUi();
    void updatePreview();

    QComboBox *m_styleComboBox;
    QComboBox *m_sortByComboBox;
    QComboBox *m_groupByComboBox;
    
    QGroupBox *m_previewGroup;
    QLabel *m_previewLabel;
};

} // namespace CodexiumMagnus::UI

#endif // BIBLIOGRAPHYSETTINGSWIDGET_H

