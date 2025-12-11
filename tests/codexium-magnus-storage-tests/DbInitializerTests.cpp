#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QTemporaryDir>
#include "DbInitializer.h"

using namespace CodexiumMagnus::Storage;

class DbInitializerTests : public QObject {
    Q_OBJECT

private slots:
    void ensureCreated_IsIdempotent_AndCreatesTables();
};

void DbInitializerTests::ensureCreated_IsIdempotent_AndCreatesTables() {
    // Arrange: create temporary directory
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString dbPath = tempDir.filePath("app.db");

    // Act: call twice, as app does on startup
    DbInitializer::ensureCreated(dbPath);
    DbInitializer::ensureCreated(dbPath);

    // Assert: open and check tables
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "test_connection");
    db.setDatabaseName(dbPath);
    QVERIFY(db.open());

    QSqlQuery query(db);
    
    // Check Cartridges table
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    query.addBindValue("Cartridges");
    QVERIFY(query.exec());
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), QString("Cartridges"));

    // Check BibliographyEntries table
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    query.addBindValue("BibliographyEntries");
    QVERIFY(query.exec());
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), QString("BibliographyEntries"));

    // Check UserSettings table
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    query.addBindValue("UserSettings");
    QVERIFY(query.exec());
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), QString("UserSettings"));

    db.close();
    QSqlDatabase::removeDatabase("test_connection");
}

QTEST_MAIN(DbInitializerTests)
#include "DbInitializerTests.moc"
