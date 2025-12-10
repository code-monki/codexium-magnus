using System;
using System.IO;
using Microsoft.Data.Sqlite;

namespace CodexiumMagnus.Storage
{
    /// <summary>
    /// Creates and migrates the local SQLite database used by Codexium Magnus.
    /// Idempotent: safe to call on every startup.
    /// </summary>
    public static class DbInitializer
    {
        // ~/.codexium-magnus/app.db  (mac / linux)
        // %LOCALAPPDATA%\\CodexiumMagnus\\app.db (windows) ← we can add later
        public static string GetDefaultDbPath()
        {
            var home = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            var appDir = Path.Combine(home, ".codexium-magnus");
            Directory.CreateDirectory(appDir);
            return Path.Combine(appDir, "app.db");
        }

        public static void EnsureCreated(string? dbPath = null)
        {
            dbPath ??= GetDefaultDbPath();
            var connString = $"Data Source={dbPath}";

            using var conn = new SqliteConnection(connString);
            conn.Open();

            // minimal schema for now — cartridges, bibliography, user settings
            using var cmd = conn.CreateCommand();
            cmd.CommandText = @"
CREATE TABLE IF NOT EXISTS Cartridges (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL,
    Path TEXT NOT NULL,
    Mounted INTEGER NOT NULL DEFAULT 0,
    LastScannedUtc TEXT
);

CREATE TABLE IF NOT EXISTS BibliographyEntries (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    CanonicalKey TEXT,
    Title TEXT,
    Author TEXT,
    Year TEXT,
    SourceId TEXT
);

CREATE TABLE IF NOT EXISTS UserSettings (
    Key TEXT PRIMARY KEY,
    Value TEXT NOT NULL,
    UpdatedUtc TEXT NOT NULL
);
";
            cmd.ExecuteNonQuery();
        }
    }
}