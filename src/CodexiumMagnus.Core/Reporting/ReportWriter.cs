using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.Json;

namespace CodexiumMagnus.Core.Reporting
{
    /// <summary>
    /// Central writer for all reports so build, test, and runtime tools look the same.
    /// </summary>
    public sealed class ReportWriter
    {
        private readonly List<ReportEntry> _entries = new();

        public IReadOnlyList<ReportEntry> Entries => _entries;

        public void Add(ReportEntry entry)
        {
            if (entry is null) throw new ArgumentNullException(nameof(entry));
            _entries.Add(entry);
        }

        public void Add(ReportSeverity severity, string title, string? details = null, string? source = null)
        {
            _entries.Add(new ReportEntry
            {
                Severity = severity,
                Title = title,
                Details = details,
                Source = source
            });
        }

        public void WriteMarkdown(string path)
        {
            Directory.CreateDirectory(Path.GetDirectoryName(path)!);

            var sb = new StringBuilder();
            sb.AppendLine("# Codexium Magnus Report");
            sb.AppendLine();
            sb.AppendLine($"Generated: {DateTime.UtcNow:O}");
            sb.AppendLine();
            sb.AppendLine("| Time (UTC) | Severity | Source | Title |");
            sb.AppendLine("|------------|----------|--------|-------|");

            foreach (var e in _entries)
            {
                sb.Append("| ")
                  .Append(e.TimestampUtc.ToString("O"))
                  .Append(" | ")
                  .Append(e.Severity)
                  .Append(" | ")
                  .Append(string.IsNullOrWhiteSpace(e.Source) ? "-" : e.Source)
                  .Append(" | ")
                  .Append(e.Title.Replace("|", "/"))
                  .AppendLine(" |");
            }

            File.WriteAllText(path, sb.ToString(), Encoding.UTF8);
        }

        public void WriteJson(string path)
        {
            Directory.CreateDirectory(Path.GetDirectoryName(path)!);

            var json = JsonSerializer.Serialize(_entries, new JsonSerializerOptions
            {
                WriteIndented = true
            });

            File.WriteAllText(path, json, Encoding.UTF8);
        }
    }
}