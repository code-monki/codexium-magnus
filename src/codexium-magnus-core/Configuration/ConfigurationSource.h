#ifndef CONFIGURATIONSOURCE_H
#define CONFIGURATIONSOURCE_H

#include "../Models/TypographyConfig.h"
#include "../Models/BibliographyConfig.h"

namespace CodexiumMagnus::Core::Configuration {

using namespace CodexiumMagnus::Core::Models;

class ConfigurationSource {
public:
    virtual ~ConfigurationSource() = default;
    virtual TypographyConfig* getTypography() = 0;
    virtual BibliographyConfig* getBibliography() = 0;
};

} // namespace CodexiumMagnus::Core::Configuration

#endif // CONFIGURATIONSOURCE_H