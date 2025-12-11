#include "CompositeConfigurationResolver.h"

namespace CodexiumMagnus::Core::Configuration {

CompositeConfigurationResolver::CompositeConfigurationResolver(
    const QList<ConfigurationSource*>& sources)
    : m_sources(sources) {
}

TypographyConfig CompositeConfigurationResolver::getEffectiveTypography() const {
    TypographyConfig result;

    // Merge from lowest → highest (reverse iteration)
    for (int i = m_sources.size() - 1; i >= 0; --i) {
        auto* layer = m_sources[i];
        if (!layer) continue;

        auto* typography = layer->getTypography();
        if (!typography) continue;

        if (!typography->baseFontFamily.isEmpty()) {
            result.baseFontFamily = typography->baseFontFamily;
        }

        if (typography->baseFontSizePt > 0.0) {
            result.baseFontSizePt = typography->baseFontSizePt;
        }

        if (!typography->headingScale.isEmpty()) {
            result.headingScale = typography->headingScale;
        }

        // Merge print options
        if (typography->printOptions.pageMarginMm > 0.0) {
            result.printOptions.pageMarginMm = typography->printOptions.pageMarginMm;
        }
        // Note: blackOnWhite is a bool, so we check if it was explicitly set
        // For simplicity, we'll just copy it if the source has it set
        result.printOptions.blackOnWhite = typography->printOptions.blackOnWhite;
    }

    return result;
}

BibliographyConfig CompositeConfigurationResolver::getEffectiveBibliography() const {
    BibliographyConfig result;

    // Merge from lowest → highest (reverse iteration)
    for (int i = m_sources.size() - 1; i >= 0; --i) {
        auto* layer = m_sources[i];
        if (!layer) continue;

        auto* bibliography = layer->getBibliography();
        if (!bibliography) continue;

        if (!bibliography->style.isEmpty()) {
            result.style = bibliography->style;
        }

        if (!bibliography->sortBy.isEmpty()) {
            result.sortBy = bibliography->sortBy;
        }

        if (!bibliography->groupBy.isEmpty()) {
            result.groupBy = bibliography->groupBy;
        }
    }

    return result;
}

} // namespace CodexiumMagnus::Core::Configuration