#include "SettingsDialog.h"
#include "TypographySettingsWidget.h"
#include "BibliographySettingsWidget.h"
#include <QVBoxLayout>
#include <QSettings>
#include <QDebug>
#include <QPushButton>

namespace CodexiumMagnus::UI {

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_tabWidget(nullptr)
    , m_typographyWidget(nullptr)
    , m_bibliographyWidget(nullptr)
    , m_buttonBox(nullptr)
{
    setupUi();
    loadSettings();
}

SettingsDialog::~SettingsDialog() {
    // Qt parent system handles cleanup
}

void SettingsDialog::setupUi() {
    setWindowTitle("Settings");
    setMinimumSize(600, 500);
    resize(700, 600);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget(this);
    
    // Typography tab
    m_typographyWidget = new TypographySettingsWidget(this);
    m_tabWidget->addTab(m_typographyWidget, "Typography");
    
    // Bibliography tab
    m_bibliographyWidget = new BibliographySettingsWidget(this);
    m_tabWidget->addTab(m_bibliographyWidget, "Bibliography");
    
    mainLayout->addWidget(m_tabWidget);
    
    // Buttons
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);
    mainLayout->addWidget(m_buttonBox);
    
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::onRejected);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &SettingsDialog::onAccepted);
}

Core::Models::TypographyConfig SettingsDialog::getTypographyConfig() const {
    return m_typographyWidget->getConfig();
}

Core::Models::BibliographyConfig SettingsDialog::getBibliographyConfig() const {
    return m_bibliographyWidget->getConfig();
}

void SettingsDialog::setTypographyConfig(const Core::Models::TypographyConfig& config) {
    m_typographyWidget->setConfig(config);
}

void SettingsDialog::setBibliographyConfig(const Core::Models::BibliographyConfig& config) {
    m_bibliographyWidget->setConfig(config);
}

void SettingsDialog::onAccepted() {
    saveSettings();
    emit settingsAccepted(getTypographyConfig(), getBibliographyConfig());
    if (sender() == m_buttonBox) {
        accept();
    }
}

void SettingsDialog::saveSettingsNow() {
    saveSettings();
}

void SettingsDialog::onRejected() {
    reject();
}

void SettingsDialog::loadSettings() {
    QSettings settings("CodexiumMagnus", "Settings");
    
    // Load typography settings
    Core::Models::TypographyConfig typography;
    typography.baseFontFamily = settings.value("typography/fontFamily", "system-ui").toString();
    typography.baseFontSizePt = settings.value("typography/fontSize", 12.0).toDouble();
    
    // Load heading scales
    QList<QVariant> scales = settings.value("typography/headingScales").toList();
    if (scales.isEmpty()) {
        // Default scales
        typography.headingScale = {2.0, 1.75, 1.5, 1.25, 1.1, 1.0};
    } else {
        typography.headingScale.clear();
        for (const QVariant& scale : scales) {
            typography.headingScale.append(scale.toDouble());
        }
    }
    
    typography.printOptions.pageMarginMm = settings.value("typography/printMargin", 10.0).toDouble();
    typography.printOptions.blackOnWhite = settings.value("typography/blackOnWhite", false).toBool();
    
    setTypographyConfig(typography);
    
    // Load bibliography settings
    Core::Models::BibliographyConfig bibliography;
    bibliography.style = settings.value("bibliography/style", "APA").toString();
    bibliography.sortBy = settings.value("bibliography/sortBy", "author").toString();
    bibliography.groupBy = settings.value("bibliography/groupBy", "").toString();
    
    setBibliographyConfig(bibliography);
}

void SettingsDialog::saveSettings() {
    QSettings settings("CodexiumMagnus", "Settings");
    
    Core::Models::TypographyConfig typography = getTypographyConfig();
    settings.setValue("typography/fontFamily", typography.baseFontFamily);
    settings.setValue("typography/fontSize", typography.baseFontSizePt);
    
    QList<QVariant> scales;
    for (double scale : typography.headingScale) {
        scales.append(scale);
    }
    settings.setValue("typography/headingScales", scales);
    
    settings.setValue("typography/printMargin", typography.printOptions.pageMarginMm);
    settings.setValue("typography/blackOnWhite", typography.printOptions.blackOnWhite);
    
    Core::Models::BibliographyConfig bibliography = getBibliographyConfig();
    settings.setValue("bibliography/style", bibliography.style);
    settings.setValue("bibliography/sortBy", bibliography.sortBy);
    settings.setValue("bibliography/groupBy", bibliography.groupBy);
    
    settings.sync();
}

} // namespace CodexiumMagnus::UI

