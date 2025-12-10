// filepath: src/CodexiumMagnus.Core/Configuration/CompositeConfigurationResolver.cs
using System.Collections.Generic;
using CodexiumMagnus.Core.Models;

namespace CodexiumMagnus.Core.Configuration
{
    /// <summary>
    /// Resolves configuration across multiple layers.
    /// Expected precedence (highest → lowest):
    ///   Session → User Profile → Corpus → System
    ///
    /// Callers may pass the sources in that order.
    /// We always merge from lowest → highest so higher layers win.
    /// </summary>
    public sealed class CompositeConfigurationResolver
    {
        private readonly IReadOnlyList<IConfigurationSource> _sources;

        public CompositeConfigurationResolver(IReadOnlyList<IConfigurationSource> sources)
        {
            _sources = sources;
        }

        public TypographyConfig GetEffectiveTypography()
        {
            var result = new TypographyConfig();

            // merge from lowest → highest
            for (int i = _sources.Count - 1; i >= 0; i--)
            {
                var layer = _sources[i].GetTypography();
                if (layer is null) continue;

                if (!string.IsNullOrWhiteSpace(layer.BaseFontFamily))
                    result.BaseFontFamily = layer.BaseFontFamily;

                if (layer.BaseFontSizePt.HasValue)
                    result.BaseFontSizePt = layer.BaseFontSizePt;

                if (layer.HeadingScale is not null && layer.HeadingScale.Count > 0)
                    result.HeadingScale = layer.HeadingScale;

                if (layer.PrintOptions is not null)
                    result.PrintOptions = layer.PrintOptions;
            }

            return result;
        }

        public BibliographyConfig GetEffectiveBibliography()
        {
            var result = new BibliographyConfig();

            // merge from lowest → highest
            for (int i = _sources.Count - 1; i >= 0; i--)
            {
                var layer = _sources[i].GetBibliography();
                if (layer is null) continue;

                if (!string.IsNullOrWhiteSpace(layer.Style))
                    result.Style = layer.Style;

                if (!string.IsNullOrWhiteSpace(layer.SortBy))
                    result.SortBy = layer.SortBy;

                if (!string.IsNullOrWhiteSpace(layer.GroupBy))
                    result.GroupBy = layer.GroupBy;
            }

            return result;
        }
    }
}