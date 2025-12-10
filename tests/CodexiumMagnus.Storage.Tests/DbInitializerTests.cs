using System;
using System.IO;
using Microsoft.Data.Sqlite;
using CodexiumMagnus.Storage;
using Xunit;

namespace CodexiumMagnus.Storage.Tests
{
    public class DbInitializerTests
    {
        [Fact]
        public void EnsureCreated_IsIdempotent_AndCreatesTables()
        {
            // Arrange: make a truly temp path
            var tempDir = Path.Combine(Path.GetTempPath(), "cm-storage-tests-" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(tempDir);
            var dbPath = Path.Combine(tempDir, "app.db");

            // Act: call twice, as app does on startup
            DbInitializer.EnsureCreated(dbPath);
            DbInitializer.EnsureCreated(dbPath);

            // Assert: open and check tables
            using var conn = new SqliteConnection($"Data Source={dbPath}");
            conn.Open();

            bool HasTable(string name)
            {
                using var cmd = conn.CreateCommand();
                cmd.CommandText = "SELECT name FROM sqlite_master WHERE type='table' AND name=@n;";
                cmd.Parameters.AddWithValue("@n", name);
                var result = cmd.ExecuteScalar();
                return result is string;
            }

            Assert.True(HasTable("Cartridges"), "Cartridges table should exist");
            Assert.True(HasTable("BibliographyEntries"), "BibliographyEntries table should exist");
            Assert.True(HasTable("UserSettings"), "UserSettings table should exist");
        }
    }
}