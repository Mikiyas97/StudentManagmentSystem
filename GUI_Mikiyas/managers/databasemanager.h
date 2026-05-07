#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>

class DatabaseManager {
public:
    static bool initDatabase();

private:
    // ═══════════════════════════════════════════════════════
    //  MySQL Connection Configuration
    // ═══════════════════════════════════════════════════════
    static const QString DB_HOST;
    static const int     DB_PORT;
    static const QString DB_NAME;
    static const QString DB_USER;
    static const QString DB_PASSWORD;
    // ═══════════════════════════════════════════════════════

    static bool createSchema();
    static bool seedInitialData();
};

#endif // DATABASEMANAGER_H
