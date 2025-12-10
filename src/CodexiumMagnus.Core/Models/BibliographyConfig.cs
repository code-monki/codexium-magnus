namespace CodexiumMagnus.Core.Models
{
    /// <summary>
    /// Bibliography-level configuration (format + sorting).
    /// </summary>
    public sealed class BibliographyConfig
    {
        /// <summary>
        /// e.g. "APA", "CMS"
        /// </summary>
        public string? Style { get; set; }

        /// <summary>
        /// e.g. "author", "title", "year"
        /// </summary>
        public string? SortBy { get; set; }

        /// <summary>
        /// Optional grouping key, e.g. "year"
        /// </summary>
        public string? GroupBy { get; set; }
    }
}