#ifndef REPORTENTRY_H
#define REPORTENTRY_H

#include <QDateTime>
#include <QString>
#include "ReportSeverity.h"

namespace CodexiumMagnus::Core::Reporting {

class ReportEntry {
public:
    QDateTime timestampUtc = QDateTime::currentDateTimeUtc();
    ReportSeverity severity;
    QString title;
    QString details;
    QString source;
};

} // namespace CodexiumMagnus::Core::Reporting

#endif // REPORTENTRY_H