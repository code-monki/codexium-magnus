#ifndef TYPOGRAPHYSETTINGSWIDGET_H
#define TYPOGRAPHYSETTINGSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include "../../codexium-magnus-core/Models/TypographyConfig.h"

namespace CodexiumMagnus::UI {

/**
 * Widget for configuring typography settings (FR-11).
 * 
 * Allows users to adjust:
 * - Base font family
 * - Base font size (points)
 * - Heading scale factors
 * - Print options (margins, black-on-white)
 */
class TypographySettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit TypographySettingsWidget(QWidget *parent = nullptr);
    ~TypographySettingsWidget();

    /**
     * Get the current typography configuration.
     * @return TypographyConfig with current widget values
     */
    Core::Models::TypographyConfig getConfig() const;

    /**
     * Set the typography configuration to display.
     * @param config TypographyConfig to display
     */
    void setConfig(const Core::Models::TypographyConfig& config);

private slots:
    void onFontFamilyChanged();
    void onFontSizeChanged(double value);
    void onHeadingScaleChanged(int index, double value);
    void onPrintMarginChanged(double value);
    void onBlackOnWhiteToggled(bool checked);

private:
    void setupUi();
    void updatePreview();

    // Base typography
    QLineEdit *m_fontFamilyEdit;
    QDoubleSpinBox *m_fontSizeSpinBox;
    
    // Heading scales (H1-H6)
    QDoubleSpinBox *m_h1ScaleSpinBox;
    QDoubleSpinBox *m_h2ScaleSpinBox;
    QDoubleSpinBox *m_h3ScaleSpinBox;
    QDoubleSpinBox *m_h4ScaleSpinBox;
    QDoubleSpinBox *m_h5ScaleSpinBox;
    QDoubleSpinBox *m_h6ScaleSpinBox;
    
    // Print options
    QDoubleSpinBox *m_pageMarginSpinBox;
    QCheckBox *m_blackOnWhiteCheckBox;
    
    // Preview
    QGroupBox *m_previewGroup;
    QLabel *m_previewLabel;
};

} // namespace CodexiumMagnus::UI

#endif // TYPOGRAPHYSETTINGSWIDGET_H

