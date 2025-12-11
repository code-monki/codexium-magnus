#ifndef COMPOSITECONFIGURATIONRESOLVER_H
#define COMPOSITECONFIGURATIONRESOLVER_H

#include <QList>
#include "ConfigurationSource.h"
#include "../Models/TypographyConfig.h"
#include "../Models/BibliographyConfig.h"

namespace CodexiumMagnus::Core::Configuration {

/**
 * Resolves configuration across multiple layers.
 * Expected precedence (highest → lowest):
 *   Session → User Profile → Corpus → System
 *
 * Callers may pass the sources in that order.
 * We always merge from lowest → highest so higher layers win.
 */
class CompositeConfigurationResolver {
public:
    explicit CompositeConfigurationResolver(const QList<ConfigurationSource*>& sources);
    
    TypographyConfig getEffectiveTypography() const;
    BibliographyConfig getEffectiveBibliography() const;

private:
    QList<ConfigurationSource*> m_sources;
};

} // namespace CodexiumMagnus::Core::Configuration

#endif // COMPOSITECONFIGURATIONRESOLVER_H