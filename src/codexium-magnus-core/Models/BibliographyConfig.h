#ifndef BIBLIOGRAPHYCONFIG_H
#define BIBLIOGRAPHYCONFIG_H

#include <QString>

namespace CodexiumMagnus::Core::Models {

class BibliographyConfig {
public:
    QString style;      // e.g. "APA", "CMS"
    QString sortBy;     // e.g. "author", "year"
    QString groupBy;    // e.g. "year"
};

} // namespace CodexiumMagnus::Core::Models

#endif // BIBLIOGRAPHYCONFIG_H