#include "ReportWriter.h"
#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QDateTime>

namespace CodexiumMagnus::Core::Reporting {

void ReportWriter::add(const ReportEntry& entry) {
    m_entries.append(entry);
}

void ReportWriter::add(ReportSeverity severity, const QString& title, 
                       const QString& details, const QString& source) {
    ReportEntry entry;
    entry.severity = severity;
    entry.title = title;
    entry.details = details;
    entry.source = source;
    entry.timestampUtc = QDateTime::currentDateTimeUtc();
    m_entries.append(entry);
}

void ReportWriter::writeMarkdown(const QString& path) const {
    QFileInfo fileInfo(path);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << "# Codexium Magnus Report\n\n";
    out << "Generated: " << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << "\n\n";
    out << "| Time (UTC) | Severity | Source | Title |\n";
    out << "|------------|----------|--------|-------|\n";

    for (const auto& entry : m_entries) {
        QString severityStr;
        switch (entry.severity) {
            case ReportSeverity::Info:
                severityStr = "Info";
                break;
            case ReportSeverity::Warning:
                severityStr = "Warning";
                break;
            case ReportSeverity::Fatal:
                severityStr = "Fatal";
                break;
        }

        QString sourceStr = entry.source.isEmpty() ? "-" : entry.source;
        QString titleStr = entry.title;
        titleStr.replace("|", "/");

        out << "| " << entry.timestampUtc.toString(Qt::ISODate)
            << " | " << severityStr
            << " | " << sourceStr
            << " | " << titleStr
            << " |\n";
    }
}

void ReportWriter::writeJson(const QString& path) const {
    QFileInfo fileInfo(path);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QJsonArray entriesArray;
    for (const auto& entry : m_entries) {
        QJsonObject entryObj;
        entryObj["timestampUtc"] = entry.timestampUtc.toString(Qt::ISODate);
        
        QString severityStr;
        switch (entry.severity) {
            case ReportSeverity::Info:
                severityStr = "Info";
                break;
            case ReportSeverity::Warning:
                severityStr = "Warning";
                break;
            case ReportSeverity::Fatal:
                severityStr = "Fatal";
                break;
        }
        entryObj["severity"] = severityStr;
        entryObj["title"] = entry.title;
        entryObj["details"] = entry.details;
        entryObj["source"] = entry.source;
        
        entriesArray.append(entryObj);
    }

    QJsonDocument doc(entriesArray);
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
    }
}

} // namespace CodexiumMagnus::Core::Reporting