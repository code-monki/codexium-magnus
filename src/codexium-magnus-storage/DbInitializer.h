#ifndef DBINITIALIZER_H
#define DBINITIALIZER_H

#include <QString>

namespace CodexiumMagnus::Storage {

/**
 * Creates and migrates the local SQLite database used by Codexium Magnus.
 * Idempotent: safe to call on every startup.
 */
class DbInitializer {
public:
    static QString getDefaultDbPath();
    static void ensureCreated(const QString& dbPath = QString());
};

} // namespace CodexiumMagnus::Storage

#endif // DBINITIALIZER_H