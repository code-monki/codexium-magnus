#include "BibliographySettingsWidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>

namespace CodexiumMagnus::UI {

BibliographySettingsWidget::BibliographySettingsWidget(QWidget *parent)
    : QWidget(parent)
    , m_styleComboBox(nullptr)
    , m_sortByComboBox(nullptr)
    , m_groupByComboBox(nullptr)
    , m_previewGroup(nullptr)
    , m_previewLabel(nullptr)
{
    setupUi();
}

BibliographySettingsWidget::~BibliographySettingsWidget() {
    // Qt parent system handles cleanup
}

void BibliographySettingsWidget::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Bibliography settings group
    QGroupBox *settingsGroup = new QGroupBox("Bibliography Formatting", this);
    QFormLayout *settingsLayout = new QFormLayout(settingsGroup);
    
    m_styleComboBox = new QComboBox(this);
    m_styleComboBox->addItem("APA", "APA");
    m_styleComboBox->addItem("CMS", "CMS");
    m_styleComboBox->setCurrentIndex(0);
    settingsLayout->addRow("Citation Style:", m_styleComboBox);
    connect(m_styleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) { onStyleChanged(m_styleComboBox->itemData(index).toString()); });
    
    m_sortByComboBox = new QComboBox(this);
    m_sortByComboBox->addItem("Author", "author");
    m_sortByComboBox->addItem("Year", "year");
    m_sortByComboBox->addItem("Title", "title");
    m_sortByComboBox->setCurrentIndex(0);
    settingsLayout->addRow("Sort By:", m_sortByComboBox);
    connect(m_sortByComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) { onSortByChanged(m_sortByComboBox->itemData(index).toString()); });
    
    m_groupByComboBox = new QComboBox(this);
    m_groupByComboBox->addItem("None", "");
    m_groupByComboBox->addItem("By Year", "year");
    m_groupByComboBox->addItem("By Author", "author");
    m_groupByComboBox->setCurrentIndex(0);
    settingsLayout->addRow("Group By:", m_groupByComboBox);
    connect(m_groupByComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) { onGroupByChanged(m_groupByComboBox->itemData(index).toString()); });
    
    mainLayout->addWidget(settingsGroup);
    
    // Preview group
    m_previewGroup = new QGroupBox("Preview", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(m_previewGroup);
    m_previewLabel = new QLabel(this);
    m_previewLabel->setWordWrap(true);
    m_previewLabel->setTextFormat(Qt::RichText);
    m_previewLabel->setMinimumHeight(150);
    previewLayout->addWidget(m_previewLabel);
    mainLayout->addWidget(m_previewGroup);
    
    mainLayout->addStretch();
    
    updatePreview();
}

Core::Models::BibliographyConfig BibliographySettingsWidget::getConfig() const {
    Core::Models::BibliographyConfig config;
    config.style = m_styleComboBox->currentData().toString();
    config.sortBy = m_sortByComboBox->currentData().toString();
    config.groupBy = m_groupByComboBox->currentData().toString();
    return config;
}

void BibliographySettingsWidget::setConfig(const Core::Models::BibliographyConfig& config) {
    int styleIndex = m_styleComboBox->findData(config.style);
    if (styleIndex >= 0) {
        m_styleComboBox->setCurrentIndex(styleIndex);
    }
    
    int sortIndex = m_sortByComboBox->findData(config.sortBy);
    if (sortIndex >= 0) {
        m_sortByComboBox->setCurrentIndex(sortIndex);
    }
    
    int groupIndex = m_groupByComboBox->findData(config.groupBy);
    if (groupIndex >= 0) {
        m_groupByComboBox->setCurrentIndex(groupIndex);
    } else {
        m_groupByComboBox->setCurrentIndex(0); // None
    }
    
    updatePreview();
}

void BibliographySettingsWidget::onStyleChanged(const QString& style) {
    Q_UNUSED(style);
    updatePreview();
}

void BibliographySettingsWidget::onSortByChanged(const QString& sortBy) {
    Q_UNUSED(sortBy);
    updatePreview();
}

void BibliographySettingsWidget::onGroupByChanged(const QString& groupBy) {
    Q_UNUSED(groupBy);
    updatePreview();
}

void BibliographySettingsWidget::updatePreview() {
    QString style = m_styleComboBox->currentData().toString();
    QString sortBy = m_sortByComboBox->currentData().toString();
    QString groupBy = m_groupByComboBox->currentData().toString();
    
    QString preview = QString("<p><b>Style:</b> %1</p>").arg(style);
    preview += QString("<p><b>Sort By:</b> %1</p>").arg(sortBy);
    if (!groupBy.isEmpty()) {
        preview += QString("<p><b>Group By:</b> %1</p>").arg(groupBy);
    }
    
    // Example bibliography entry
    QString example;
    if (style == "APA") {
        example = "<p><i>Example (APA):</i><br/>"
                  "Smith, J. (2023). <i>Example Title</i>. Publisher.</p>";
    } else {
        example = "<p><i>Example (CMS):</i><br/>"
                  "Smith, John. <i>Example Title</i>. Publisher, 2023.</p>";
    }
    
    preview += example;
    m_previewLabel->setText(preview);
}

} // namespace CodexiumMagnus::UI

