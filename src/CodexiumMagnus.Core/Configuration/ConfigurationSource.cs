using CodexiumMagnus.Core.Models;

namespace CodexiumMagnus.Core.Configuration
{
    /// <summary>
    /// Represents one layer of configuration (system, corpus, user, session).
    /// Any value can be null; the resolver will layer them.
    /// </summary>
    public interface IConfigurationSource
    {
        TypographyConfig? GetTypography();
        BibliographyConfig? GetBibliography();
    }
}