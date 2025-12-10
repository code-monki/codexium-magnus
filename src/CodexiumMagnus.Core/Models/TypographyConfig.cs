using System.Collections.Generic;

namespace CodexiumMagnus.Core.Models
{
    /// <summary>
    /// Typography settings passed to CEFWindow.
    /// </summary>
    public sealed class TypographyConfig
    {
        public string? BaseFontFamily { get; set; }

        /// <summary>
        /// Base font size in points (or null → use system/corpus default).
        /// </summary>
        public double? BaseFontSizePt { get; set; }

        /// <summary>
        /// Optional per-heading scaling, e.g. [1.8, 1.6, 1.4, 1.2, 1.1, 1.0]
        /// </summary>
        public IList<double>? HeadingScale { get; set; }

        public TypographyPrintOptions? PrintOptions { get; set; }
    }

    public sealed class TypographyPrintOptions
    {
        public double? PageMarginMm { get; set; }
        public bool? BlackOnWhite { get; set; }
    }
}