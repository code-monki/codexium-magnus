#include "TypographySettingsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFontDatabase>
#include <QDebug>

namespace CodexiumMagnus::UI {

TypographySettingsWidget::TypographySettingsWidget(QWidget *parent)
    : QWidget(parent)
    , m_fontFamilyEdit(nullptr)
    , m_fontSizeSpinBox(nullptr)
    , m_h1ScaleSpinBox(nullptr)
    , m_h2ScaleSpinBox(nullptr)
    , m_h3ScaleSpinBox(nullptr)
    , m_h4ScaleSpinBox(nullptr)
    , m_h5ScaleSpinBox(nullptr)
    , m_h6ScaleSpinBox(nullptr)
    , m_pageMarginSpinBox(nullptr)
    , m_blackOnWhiteCheckBox(nullptr)
    , m_previewGroup(nullptr)
    , m_previewLabel(nullptr)
{
    setupUi();
}

TypographySettingsWidget::~TypographySettingsWidget() {
    // Qt parent system handles cleanup
}

void TypographySettingsWidget::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Base typography group
    QGroupBox *baseGroup = new QGroupBox("Base Typography", this);
    QFormLayout *baseLayout = new QFormLayout(baseGroup);
    
    m_fontFamilyEdit = new QLineEdit(this);
    m_fontFamilyEdit->setPlaceholderText("e.g., Inter, Roboto, system-ui");
    baseLayout->addRow("Font Family:", m_fontFamilyEdit);
    connect(m_fontFamilyEdit, &QLineEdit::textChanged, this, &TypographySettingsWidget::onFontFamilyChanged);
    
    m_fontSizeSpinBox = new QDoubleSpinBox(this);
    m_fontSizeSpinBox->setRange(8.0, 24.0);
    m_fontSizeSpinBox->setSingleStep(0.5);
    m_fontSizeSpinBox->setSuffix(" pt");
    m_fontSizeSpinBox->setValue(12.0);
    baseLayout->addRow("Base Font Size:", m_fontSizeSpinBox);
    connect(m_fontSizeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TypographySettingsWidget::onFontSizeChanged);
    
    mainLayout->addWidget(baseGroup);
    
    // Heading scales group
    QGroupBox *headingGroup = new QGroupBox("Heading Scales", this);
    QFormLayout *headingLayout = new QFormLayout(headingGroup);
    
    m_h1ScaleSpinBox = new QDoubleSpinBox(this);
    m_h1ScaleSpinBox->setRange(1.0, 4.0);
    m_h1ScaleSpinBox->setSingleStep(0.1);
    m_h1ScaleSpinBox->setValue(2.0);
    headingLayout->addRow("H1 Scale:", m_h1ScaleSpinBox);
    connect(m_h1ScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) { onHeadingScaleChanged(0, value); });
    
    m_h2ScaleSpinBox = new QDoubleSpinBox(this);
    m_h2ScaleSpinBox->setRange(1.0, 4.0);
    m_h2ScaleSpinBox->setSingleStep(0.1);
    m_h2ScaleSpinBox->setValue(1.75);
    headingLayout->addRow("H2 Scale:", m_h2ScaleSpinBox);
    connect(m_h2ScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) { onHeadingScaleChanged(1, value); });
    
    m_h3ScaleSpinBox = new QDoubleSpinBox(this);
    m_h3ScaleSpinBox->setRange(1.0, 4.0);
    m_h3ScaleSpinBox->setSingleStep(0.1);
    m_h3ScaleSpinBox->setValue(1.5);
    headingLayout->addRow("H3 Scale:", m_h3ScaleSpinBox);
    connect(m_h3ScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) { onHeadingScaleChanged(2, value); });
    
    m_h4ScaleSpinBox = new QDoubleSpinBox(this);
    m_h4ScaleSpinBox->setRange(1.0, 4.0);
    m_h4ScaleSpinBox->setSingleStep(0.1);
    m_h4ScaleSpinBox->setValue(1.25);
    headingLayout->addRow("H4 Scale:", m_h4ScaleSpinBox);
    connect(m_h4ScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) { onHeadingScaleChanged(3, value); });
    
    m_h5ScaleSpinBox = new QDoubleSpinBox(this);
    m_h5ScaleSpinBox->setRange(1.0, 4.0);
    m_h5ScaleSpinBox->setSingleStep(0.1);
    m_h5ScaleSpinBox->setValue(1.1);
    headingLayout->addRow("H5 Scale:", m_h5ScaleSpinBox);
    connect(m_h5ScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) { onHeadingScaleChanged(4, value); });
    
    m_h6ScaleSpinBox = new QDoubleSpinBox(this);
    m_h6ScaleSpinBox->setRange(1.0, 4.0);
    m_h6ScaleSpinBox->setSingleStep(0.1);
    m_h6ScaleSpinBox->setValue(1.0);
    headingLayout->addRow("H6 Scale:", m_h6ScaleSpinBox);
    connect(m_h6ScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) { onHeadingScaleChanged(5, value); });
    
    mainLayout->addWidget(headingGroup);
    
    // Print options group
    QGroupBox *printGroup = new QGroupBox("Print Options", this);
    QFormLayout *printLayout = new QFormLayout(printGroup);
    
    m_pageMarginSpinBox = new QDoubleSpinBox(this);
    m_pageMarginSpinBox->setRange(0.0, 50.0);
    m_pageMarginSpinBox->setSingleStep(1.0);
    m_pageMarginSpinBox->setSuffix(" mm");
    m_pageMarginSpinBox->setValue(10.0);
    printLayout->addRow("Page Margin:", m_pageMarginSpinBox);
    connect(m_pageMarginSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TypographySettingsWidget::onPrintMarginChanged);
    
    m_blackOnWhiteCheckBox = new QCheckBox("Force black text on white background", this);
    m_blackOnWhiteCheckBox->setChecked(false);
    printLayout->addRow("", m_blackOnWhiteCheckBox);
    connect(m_blackOnWhiteCheckBox, &QCheckBox::toggled,
            this, &TypographySettingsWidget::onBlackOnWhiteToggled);
    
    mainLayout->addWidget(printGroup);
    
    // Preview group
    m_previewGroup = new QGroupBox("Preview", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(m_previewGroup);
    m_previewLabel = new QLabel(this);
    m_previewLabel->setWordWrap(true);
    m_previewLabel->setTextFormat(Qt::RichText);
    m_previewLabel->setMinimumHeight(100);
    previewLayout->addWidget(m_previewLabel);
    mainLayout->addWidget(m_previewGroup);
    
    mainLayout->addStretch();
    
    updatePreview();
}

Core::Models::TypographyConfig TypographySettingsWidget::getConfig() const {
    Core::Models::TypographyConfig config;
    config.baseFontFamily = m_fontFamilyEdit->text();
    config.baseFontSizePt = m_fontSizeSpinBox->value();
    
    config.headingScale.clear();
    config.headingScale.append(m_h1ScaleSpinBox->value());
    config.headingScale.append(m_h2ScaleSpinBox->value());
    config.headingScale.append(m_h3ScaleSpinBox->value());
    config.headingScale.append(m_h4ScaleSpinBox->value());
    config.headingScale.append(m_h5ScaleSpinBox->value());
    config.headingScale.append(m_h6ScaleSpinBox->value());
    
    config.printOptions.pageMarginMm = m_pageMarginSpinBox->value();
    config.printOptions.blackOnWhite = m_blackOnWhiteCheckBox->isChecked();
    
    return config;
}

void TypographySettingsWidget::setConfig(const Core::Models::TypographyConfig& config) {
    m_fontFamilyEdit->setText(config.baseFontFamily);
    m_fontSizeSpinBox->setValue(config.baseFontSizePt);
    
    if (config.headingScale.size() >= 6) {
        m_h1ScaleSpinBox->setValue(config.headingScale[0]);
        m_h2ScaleSpinBox->setValue(config.headingScale[1]);
        m_h3ScaleSpinBox->setValue(config.headingScale[2]);
        m_h4ScaleSpinBox->setValue(config.headingScale[3]);
        m_h5ScaleSpinBox->setValue(config.headingScale[4]);
        m_h6ScaleSpinBox->setValue(config.headingScale[5]);
    }
    
    m_pageMarginSpinBox->setValue(config.printOptions.pageMarginMm);
    m_blackOnWhiteCheckBox->setChecked(config.printOptions.blackOnWhite);
    
    updatePreview();
}

void TypographySettingsWidget::onFontFamilyChanged() {
    updatePreview();
}

void TypographySettingsWidget::onFontSizeChanged(double value) {
    Q_UNUSED(value);
    updatePreview();
}

void TypographySettingsWidget::onHeadingScaleChanged(int index, double value) {
    Q_UNUSED(index);
    Q_UNUSED(value);
    updatePreview();
}

void TypographySettingsWidget::onPrintMarginChanged(double value) {
    Q_UNUSED(value);
    // Print options don't affect preview
}

void TypographySettingsWidget::onBlackOnWhiteToggled(bool checked) {
    Q_UNUSED(checked);
    // Print options don't affect preview
}

void TypographySettingsWidget::updatePreview() {
    QString fontFamily = m_fontFamilyEdit->text().isEmpty() ? "system-ui" : m_fontFamilyEdit->text();
    double fontSize = m_fontSizeSpinBox->value();
    
    QString preview = QString(R"(
        <div style="font-family: %1; font-size: %2pt;">
            <h1 style="font-size: %3pt;">Heading 1</h1>
            <h2 style="font-size: %4pt;">Heading 2</h2>
            <h3 style="font-size: %5pt;">Heading 3</h3>
            <p>This is a sample paragraph showing the base font size and family. 
            The typography settings will apply to all documents in the viewer.</p>
        </div>
    )")
    .arg(fontFamily)
    .arg(fontSize)
    .arg(fontSize * m_h1ScaleSpinBox->value())
    .arg(fontSize * m_h2ScaleSpinBox->value())
    .arg(fontSize * m_h3ScaleSpinBox->value());
    
    m_previewLabel->setText(preview);
}

} // namespace CodexiumMagnus::UI

