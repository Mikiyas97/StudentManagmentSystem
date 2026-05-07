#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QCryptographicHash>
#include "usermanager.h"

// Initialize static constants
const QString DatabaseManager::DB_HOST     = "localhost";
const int     DatabaseManager::DB_PORT     = 3306;
const QString DatabaseManager::DB_NAME     = "school_db";
const QString DatabaseManager::DB_USER     = "root";
const QString DatabaseManager::DB_PASSWORD = "1234";

bool DatabaseManager::initDatabase() {
    // Using ODBC driver to connect to MySQL
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    
    QString connStr = QString(
        "DRIVER={MySQL ODBC 9.7 Unicode Driver};"
        "SERVER=%1;"
        "PORT=%2;"
        "DATABASE=%3;"
        "USER=%4;"
        "PASSWORD=%5;"
        "OPTION=3;"
    ).arg(DB_HOST).arg(DB_PORT).arg(DB_NAME).arg(DB_USER).arg(DB_PASSWORD);
    
    db.setDatabaseName(connStr);
    
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", 
            "Failed to connect to MySQL!\n\n"
            "Error: " + db.lastError().text() + "\n\n"
            "Please check your settings in databasemanager.cpp");
        return false;
    }

    if (!createSchema()) return false;
    if (!seedInitialData()) return false;

    return true;
}

bool DatabaseManager::createSchema() {
    QSqlQuery query;
    
    // 1. Core Structure Tables
    query.exec("CREATE TABLE IF NOT EXISTS academic_years ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "name VARCHAR(50) UNIQUE)");

    query.exec("CREATE TABLE IF NOT EXISTS grade_levels ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "name VARCHAR(50) UNIQUE)");

    query.exec("CREATE TABLE IF NOT EXISTS streams ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "name VARCHAR(100) UNIQUE)");

    query.exec("CREATE TABLE IF NOT EXISTS sections ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "grade_id INT, "
               "name VARCHAR(50), "
               "year_id INT, "
               "FOREIGN KEY(grade_id) REFERENCES grade_levels(id), "
               "FOREIGN KEY(year_id) REFERENCES academic_years(id))");

    // 2. People Tables
    query.exec("CREATE TABLE IF NOT EXISTS students ("
               "id INT PRIMARY KEY, "
               "fullName VARCHAR(255), "
               "gender VARCHAR(20), "
               "date_of_birth VARCHAR(50), "
               "grade_id INT, "
               "section_id INT, "
               "stream_id INT, "
               "phone VARCHAR(50), "
               "email VARCHAR(255), "
               "status VARCHAR(50), "
               "FOREIGN KEY(grade_id) REFERENCES grade_levels(id), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), "
               "FOREIGN KEY(stream_id) REFERENCES streams(id))");
               
    query.exec("CREATE TABLE IF NOT EXISTS teachers ("
               "id INT PRIMARY KEY, "
               "fullName VARCHAR(255), "
               "gender VARCHAR(20), "
               "date_of_birth VARCHAR(50), "
               "phone VARCHAR(50), "
               "email VARCHAR(255), "
               "subject_id INT)");

    // 3. User Accounts
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "username VARCHAR(100) PRIMARY KEY, "
               "password VARCHAR(64), "
               "role VARCHAR(50), "
               "relatedId INT)");

    // 4. Academic Data Tables
    query.exec("CREATE TABLE IF NOT EXISTS subjects ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "name VARCHAR(255), "
               "grade_id INT, "
               "stream_id INT, "
               "FOREIGN KEY(grade_id) REFERENCES grade_levels(id), "
               "FOREIGN KEY(stream_id) REFERENCES streams(id))");

    query.exec("CREATE TABLE IF NOT EXISTS teaching_assignments ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "teacher_id INT, "
               "subject_id INT, "
               "section_id INT, "
               "year_id INT, "
               "FOREIGN KEY(teacher_id) REFERENCES teachers(id), "
               "FOREIGN KEY(subject_id) REFERENCES subjects(id), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), "
               "FOREIGN KEY(year_id) REFERENCES academic_years(id), "
               "UNIQUE(section_id, subject_id, year_id))");

    query.exec("CREATE TABLE IF NOT EXISTS homeroom_assignments ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "teacher_id INT, "
               "section_id INT, "
               "year_id INT, "
               "FOREIGN KEY(teacher_id) REFERENCES teachers(id), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), "
               "FOREIGN KEY(year_id) REFERENCES academic_years(id))");

    query.exec("CREATE TABLE IF NOT EXISTS marks ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "student_id INT, "
               "subject_id INT, "
               "section_id INT, "
               "year_id INT, "
               "semester INT DEFAULT 1, "
               "score DOUBLE DEFAULT 0, "
               "FOREIGN KEY(student_id) REFERENCES students(id), "
               "FOREIGN KEY(subject_id) REFERENCES subjects(id), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), "
               "FOREIGN KEY(year_id) REFERENCES academic_years(id))");

    query.exec("CREATE TABLE IF NOT EXISTS ranking_approvals ("
               "section_id INT, "
               "year_id INT, "
               "semester INT DEFAULT 1, "
               "is_approved TINYINT(1) DEFAULT 0, "
               "PRIMARY KEY(section_id, year_id, semester), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), "
               "FOREIGN KEY(year_id) REFERENCES academic_years(id))");

    // 5. Data Integrity Migration (Ensure years match sections)
    query.exec("UPDATE marks SET year_id = (SELECT year_id FROM sections WHERE sections.id = marks.section_id) "
               "WHERE year_id != (SELECT year_id FROM sections WHERE sections.id = marks.section_id)");
    
    query.exec("UPDATE teaching_assignments SET year_id = (SELECT year_id FROM sections WHERE sections.id = teaching_assignments.section_id) "
               "WHERE year_id != (SELECT year_id FROM sections WHERE sections.id = teaching_assignments.section_id)");

    return true;
}

bool DatabaseManager::seedInitialData() {
    QSqlQuery query;

    // Seed Academic Years
    query.exec("INSERT IGNORE INTO academic_years (name) VALUES ('2018')");
    query.exec("INSERT IGNORE INTO academic_years (name) VALUES ('2019')");
    
    // Seed Grade Levels
    for (const QString &g : QStringList{"9", "10", "11", "12"}) {
        query.exec(QString("INSERT IGNORE INTO grade_levels (name) VALUES ('%1')").arg(g));
    }

    // Seed Streams
    query.exec("INSERT IGNORE INTO streams (name) VALUES ('Natural Science')");
    query.exec("INSERT IGNORE INTO streams (name) VALUES ('Social Science')");

    // Seed default admin
    QSqlQuery checkAdmin;
    checkAdmin.prepare("SELECT * FROM users WHERE username = 'admin'");
    if (checkAdmin.exec() && !checkAdmin.next()) {
        QString hashedPass = UserManager::hashPassword("admin123");
        query.prepare("INSERT INTO users (username, password, role, relatedId) VALUES ('admin', ?, 'admin', -1)");
        query.addBindValue(hashedPass);
        query.exec();
    }

    return true;
}
