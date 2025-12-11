#ifndef REPORTWRITER_H
#define REPORTWRITER_H

#include <QList>
#include <QString>
#include "ReportEntry.h"

namespace CodexiumMagnus::Core::Reporting {

class ReportWriter {
public:
    ReportWriter() = default;
    ~ReportWriter() = default;

    void add(const ReportEntry& entry);
    void add(ReportSeverity severity, const QString& title, 
             const QString& details = QString(), 
             const QString& source = QString());

    const QList<ReportEntry>& entries() const { return m_entries; }

    void writeMarkdown(const QString& path) const;
    void writeJson(const QString& path) const;

private:
    QList<ReportEntry> m_entries;
};

} // namespace CodexiumMagnus::Core::Reporting

#endif // REPORTWRITER_H