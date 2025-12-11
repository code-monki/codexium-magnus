#include "DbInitializer.h"
#include <QStandardPaths>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace CodexiumMagnus::Storage {

QString DbInitializer::getDefaultDbPath() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDir;
    if (!appDir.exists(appDataPath)) {
        appDir.mkpath(appDataPath);
    }
    return appDir.filePath("app.db");
}

void DbInitializer::ensureCreated(const QString& dbPath) {
    QString path = dbPath.isEmpty() ? getDefaultDbPath() : dbPath;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "init_connection");
    db.setDatabaseName(path);

    if (!db.open()) {
        return; // Error opening database
    }

    QSqlQuery query(db);

    // Create Cartridges table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS Cartridges (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL,
            Path TEXT NOT NULL,
            Mounted INTEGER NOT NULL DEFAULT 0,
            LastScannedUtc TEXT
        )
    )");

    // Create BibliographyEntries table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS BibliographyEntries (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            CanonicalKey TEXT,
            Title TEXT,
            Author TEXT,
            Year TEXT,
            SourceId TEXT
        )
    )");

    // Create UserSettings table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS UserSettings (
            Key TEXT PRIMARY KEY,
            Value TEXT NOT NULL,
            UpdatedUtc TEXT NOT NULL
        )
    )");

    db.close();
    QSqlDatabase::removeDatabase("init_connection");
}

} // namespace CodexiumMagnus::Storage