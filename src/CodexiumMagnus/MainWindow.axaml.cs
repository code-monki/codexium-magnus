// filepath: src/CodexiumMagnus/MainWindow.axaml.cs
using System.Text.Json;
using Avalonia.Controls;
using CodexiumMagnus.Core.Configuration;
using CodexiumMagnus.Services;

namespace CodexiumMagnus
{
    public partial class MainWindow : Window
    {
        private readonly CompositeConfigurationResolver _resolver;
        private readonly ICefBridge _cef;

        public MainWindow()
        {
            InitializeComponent();

            // temporary config sources so the resolver has something to merge
            var system = new SystemConfigSource();
            var corpus = new CorpusConfigSource();
            var user = new UserProfileConfigSource();
            var session = new SessionConfigSource();

            _resolver = new CompositeConfigurationResolver(
                new IConfigurationSource[]
                {
                    // order matters: Session → User → Corpus → System
                    session,
                    user,
                    corpus,
                    system
                }
            );

            _cef = new CefBridgeStub();

            this.Opened += (_, _) => PushConfigToCef();
        }

        private void PushConfigToCef()
        {
            var typography = _resolver.GetEffectiveTypography();
            var bibliography = _resolver.GetEffectiveBibliography();

            var payload = new
            {
                type = "config:update",
                typography,
                bibliography
            };

            var json = JsonSerializer.Serialize(payload);
            _cef.Send(json);
        }
    }

    // --- placeholder sources so this compiles ---

    internal sealed class SystemConfigSource : IConfigurationSource
    {
        public Core.Models.TypographyConfig? GetTypography() =>
            new() { BaseFontFamily = "Segoe UI", BaseFontSizePt = 12 };

        public Core.Models.BibliographyConfig? GetBibliography() =>
            new() { Style = "APA" };
    }

    internal sealed class CorpusConfigSource : IConfigurationSource
    {
        public Core.Models.TypographyConfig? GetTypography() => null;
        public Core.Models.BibliographyConfig? GetBibliography() => null;
    }

    internal sealed class UserProfileConfigSource : IConfigurationSource
    {
        public Core.Models.TypographyConfig? GetTypography() =>
            new() { BaseFontSizePt = 11.5 };

        public Core.Models.BibliographyConfig? GetBibliography() => null;
    }

    internal sealed class SessionConfigSource : IConfigurationSource
    {
        public Core.Models.TypographyConfig? GetTypography() => null;
        public Core.Models.BibliographyConfig? GetBibliography() => null;
    }
}