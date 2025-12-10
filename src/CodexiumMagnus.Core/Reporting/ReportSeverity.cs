namespace CodexiumMagnus.Core.Reporting
{
    /// <summary>
    /// Severity levels for all generated reports, tests, and sanity checks.
    /// Keep in sync with tools that emit Markdown/JSON.
    /// </summary>
    public enum ReportSeverity
    {
        Info = 0,
        Warning = 1,
        Fatal = 2
    }
}