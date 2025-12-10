using System.Collections.Generic;
using CodexiumMagnus.Core.Configuration;
using CodexiumMagnus.Core.Models;
using Xunit;

namespace CodexiumMagnus.Core.Tests.Configuration
{
    public class CompositeConfigurationResolverTests
    {
        // helper sources to make the tests readable
        private sealed class SimpleSource : IConfigurationSource
        {
            private readonly TypographyConfig? _typ;
            private readonly BibliographyConfig? _bib;

            public SimpleSource(TypographyConfig? typ = null, BibliographyConfig? bib = null)
            {
                _typ = typ;
                _bib = bib;
            }

            public TypographyConfig? GetTypography() => _typ;
            public BibliographyConfig? GetBibliography() => _bib;
        }

        [Fact]
        public void Typography_HigherLayer_OverridesLowerLayer()
        {
            // arrange
            var system = new SimpleSource(
                new TypographyConfig
                {
                    BaseFontFamily = "SystemFont",
                    BaseFontSizePt = 12
                });

            var user = new SimpleSource(
                new TypographyConfig
                {
                    BaseFontSizePt = 14 // user wants bigger
                });

            var session = new SimpleSource(
                new TypographyConfig
                {
                    BaseFontFamily = "SessionFont" // temporary override
                });

            var resolver = new CompositeConfigurationResolver(new List<IConfigurationSource>
            {
                // order: Session → User → Corpus → System
                session,
                user,
                system
            });

            // act
            var effective = resolver.GetEffectiveTypography();

            // assert
            Assert.Equal("SessionFont", effective.BaseFontFamily);
            Assert.Equal(14, effective.BaseFontSizePt);
        }

        [Fact]
        public void Bibliography_UsesSystem_WhenNoOtherLayers()
        {
            var system = new SimpleSource(
                bib: new BibliographyConfig
                {
                    Style = "APA",
                    SortBy = "author"
                });

            var resolver = new CompositeConfigurationResolver(new List<IConfigurationSource>
            {
                system
            });

            var effective = resolver.GetEffectiveBibliography();

            Assert.Equal("APA", effective.Style);
            Assert.Equal("author", effective.SortBy);
            Assert.Null(effective.GroupBy);
        }

        [Fact]
        public void Bibliography_Session_CanOverride_SortOnly()
        {
            var system = new SimpleSource(
                bib: new BibliographyConfig
                {
                    Style = "APA",
                    SortBy = "author"
                });

            var user = new SimpleSource(
                bib: new BibliographyConfig
                {
                    Style = "CMS" // user prefers CMS
                });

            var session = new SimpleSource(
                bib: new BibliographyConfig
                {
                    SortBy = "year" // temporary view
                });

            var resolver = new CompositeConfigurationResolver(new List<IConfigurationSource>
            {
                session,
                user,
                system
            });

            var effective = resolver.GetEffectiveBibliography();

            // session wins for sort
            Assert.Equal("year", effective.SortBy);
            // user wins for style
            Assert.Equal("CMS", effective.Style);
        }
    }
}