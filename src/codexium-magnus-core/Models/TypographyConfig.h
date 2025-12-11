#ifndef TYPOGRAPHYCONFIG_H
#define TYPOGRAPHYCONFIG_H

#include <QString>
#include <QList>

namespace CodexiumMagnus::Core::Models {

class TypographyPrintOptions {
public:
    double pageMarginMm = 0.0;
    bool blackOnWhite = false;
};

class TypographyConfig {
public:
    QString baseFontFamily;
    double baseFontSizePt = 0.0;
    QList<double> headingScale;
    TypographyPrintOptions printOptions;
};

} // namespace CodexiumMagnus::Core::Models

#endif // TYPOGRAPHYCONFIG_H