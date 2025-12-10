using System;

namespace CodexiumMagnus.Core.Reporting
{
    /// <summary>
    /// A single report line describing one issue, test, or build step.
    /// </summary>
    public sealed class ReportEntry
    {
        public DateTime TimestampUtc { get; init; } = DateTime.UtcNow;

        public ReportSeverity Severity { get; init; }

        /// <summary>
        /// Short, human-readable label, e.g. "Typography config missing".
        /// </summary>
        public string Title { get; init; } = string.Empty;

        /// <summary>
        /// Longer description with context (file, step, rule).
        /// </summary>
        public string? Details { get; init; }

        /// <summary>
        /// Optional: which component raised it (e.g. "Pipeline", "Playwright", "Avalonia").
        /// </summary>
        public string? Source { get; init; }
    }
}