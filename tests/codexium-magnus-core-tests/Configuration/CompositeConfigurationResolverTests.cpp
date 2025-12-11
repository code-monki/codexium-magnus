#include <QtTest/QtTest>
#include "Configuration/CompositeConfigurationResolver.h"
#include "Configuration/ConfigurationSource.h"
#include "Models/TypographyConfig.h"
#include "Models/BibliographyConfig.h"

using namespace CodexiumMagnus::Core::Configuration;
using namespace CodexiumMagnus::Core::Models;

// Helper source for testing
class SimpleSource : public ConfigurationSource {
public:
    SimpleSource(TypographyConfig* typ = nullptr, BibliographyConfig* bib = nullptr)
        : m_typ(typ), m_bib(bib) {}

    TypographyConfig* getTypography() override { return m_typ; }
    BibliographyConfig* getBibliography() override { return m_bib; }

private:
    TypographyConfig* m_typ;
    BibliographyConfig* m_bib;
};

class CompositeConfigurationResolverTests : public QObject {
    Q_OBJECT

private slots:
    void typography_HigherLayer_OverridesLowerLayer();
    void bibliography_UsesSystem_WhenNoOtherLayers();
    void bibliography_Session_CanOverride_SortOnly();
};

void CompositeConfigurationResolverTests::typography_HigherLayer_OverridesLowerLayer() {
    // Arrange
    auto systemTyp = new TypographyConfig();
    systemTyp->baseFontFamily = "SystemFont";
    systemTyp->baseFontSizePt = 12;
    SimpleSource system(systemTyp, nullptr);

    auto userTyp = new TypographyConfig();
    userTyp->baseFontSizePt = 14; // user wants bigger
    SimpleSource user(userTyp, nullptr);

    auto sessionTyp = new TypographyConfig();
    sessionTyp->baseFontFamily = "SessionFont"; // temporary override
    SimpleSource session(sessionTyp, nullptr);

    QList<ConfigurationSource*> sources = {&session, &user, &system};
    CompositeConfigurationResolver resolver(sources);

    // Act
    TypographyConfig effective = resolver.getEffectiveTypography();

    // Assert
    QCOMPARE(effective.baseFontFamily, QString("SessionFont"));
    QCOMPARE(effective.baseFontSizePt, 14.0);
}

void CompositeConfigurationResolverTests::bibliography_UsesSystem_WhenNoOtherLayers() {
    auto systemBib = new BibliographyConfig();
    systemBib->style = "APA";
    systemBib->sortBy = "author";
    SimpleSource system(nullptr, systemBib);

    QList<ConfigurationSource*> sources = {&system};
    CompositeConfigurationResolver resolver(sources);

    BibliographyConfig effective = resolver.getEffectiveBibliography();

    QCOMPARE(effective.style, QString("APA"));
    QCOMPARE(effective.sortBy, QString("author"));
    QVERIFY(effective.groupBy.isEmpty());
}

void CompositeConfigurationResolverTests::bibliography_Session_CanOverride_SortOnly() {
    auto systemBib = new BibliographyConfig();
    systemBib->style = "APA";
    systemBib->sortBy = "author";
    SimpleSource system(nullptr, systemBib);

    auto userBib = new BibliographyConfig();
    userBib->style = "CMS"; // user prefers CMS
    SimpleSource user(nullptr, userBib);

    auto sessionBib = new BibliographyConfig();
    sessionBib->sortBy = "year"; // temporary view
    SimpleSource session(nullptr, sessionBib);

    QList<ConfigurationSource*> sources = {&session, &user, &system};
    CompositeConfigurationResolver resolver(sources);

    BibliographyConfig effective = resolver.getEffectiveBibliography();

    // Session wins for sort
    QCOMPARE(effective.sortBy, QString("year"));
    // User wins for style
    QCOMPARE(effective.style, QString("CMS"));
}

QTEST_MAIN(CompositeConfigurationResolverTests)
#include "CompositeConfigurationResolverTests.moc"
