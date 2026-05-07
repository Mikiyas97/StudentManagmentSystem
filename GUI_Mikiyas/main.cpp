#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "logindialog.h"
#include "mainwindow.h"

bool setupDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("school.db");
    
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", "Failed to open database!");
        return false;
    }
    
    QSqlQuery query;
    
    // 1. Core Structure Tables
    query.exec("CREATE TABLE IF NOT EXISTS academic_years (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE)");
    query.exec("CREATE TABLE IF NOT EXISTS grade_levels (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE)");
    query.exec("CREATE TABLE IF NOT EXISTS streams (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE)");
    query.exec("CREATE TABLE IF NOT EXISTS sections (id INTEGER PRIMARY KEY AUTOINCREMENT, grade_id INTEGER, name TEXT, year_id INTEGER, "
               "FOREIGN KEY(grade_id) REFERENCES grade_levels(id), FOREIGN KEY(year_id) REFERENCES academic_years(id))");

    // 2. People Tables
    query.exec("CREATE TABLE IF NOT EXISTS students (id INTEGER PRIMARY KEY, fullName TEXT, gender TEXT, date_of_birth TEXT, grade_id INTEGER, section_id INTEGER, stream_id INTEGER, "
               "phone TEXT, email TEXT, status TEXT, "
               "FOREIGN KEY(grade_id) REFERENCES grade_levels(id), FOREIGN KEY(section_id) REFERENCES sections(id), FOREIGN KEY(stream_id) REFERENCES streams(id))");
    // Migration: ensure new columns exist
    query.exec("ALTER TABLE students ADD COLUMN gender TEXT");
    query.exec("ALTER TABLE students ADD COLUMN date_of_birth TEXT");
               
    query.exec("CREATE TABLE IF NOT EXISTS teachers (id INTEGER PRIMARY KEY, fullName TEXT, gender TEXT, date_of_birth TEXT, phone TEXT, email TEXT, subject_id INTEGER, "
               "FOREIGN KEY(subject_id) REFERENCES subjects(id))");
    query.exec("ALTER TABLE teachers ADD COLUMN gender TEXT");
    query.exec("ALTER TABLE teachers ADD COLUMN date_of_birth TEXT");
    // Migration: ensure subject_id exists if table was already there
    query.exec("ALTER TABLE teachers ADD COLUMN subject_id INTEGER");

    // 3. User Accounts
    query.exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT, role TEXT, relatedId INTEGER)");

    // 4. Academic Data Tables
    query.exec("CREATE TABLE IF NOT EXISTS subjects (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, grade_id INTEGER, stream_id INTEGER, "
               "FOREIGN KEY(grade_id) REFERENCES grade_levels(id), FOREIGN KEY(stream_id) REFERENCES streams(id))");

    query.exec("CREATE TABLE IF NOT EXISTS teaching_assignments (id INTEGER PRIMARY KEY AUTOINCREMENT, teacher_id INTEGER, subject_id INTEGER, section_id INTEGER, year_id INTEGER, "
               "FOREIGN KEY(teacher_id) REFERENCES teachers(id), FOREIGN KEY(subject_id) REFERENCES subjects(id), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), FOREIGN KEY(year_id) REFERENCES academic_years(id), "
               "UNIQUE(section_id, subject_id, year_id))");

    query.exec("CREATE TABLE IF NOT EXISTS homeroom_assignments (id INTEGER PRIMARY KEY AUTOINCREMENT, teacher_id INTEGER, section_id INTEGER, year_id INTEGER, "
               "FOREIGN KEY(teacher_id) REFERENCES teachers(id), FOREIGN KEY(section_id) REFERENCES sections(id), FOREIGN KEY(year_id) REFERENCES academic_years(id))");

    query.exec("CREATE TABLE IF NOT EXISTS marks (id INTEGER PRIMARY KEY AUTOINCREMENT, student_id INTEGER, subject_id INTEGER, section_id INTEGER, year_id INTEGER, semester INTEGER DEFAULT 1, score REAL, "
               "FOREIGN KEY(student_id) REFERENCES students(id), FOREIGN KEY(subject_id) REFERENCES subjects(id), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), FOREIGN KEY(year_id) REFERENCES academic_years(id))");
    query.exec("ALTER TABLE marks ADD COLUMN semester INTEGER DEFAULT 1");

    query.exec("CREATE TABLE IF NOT EXISTS ranking_approvals (section_id INTEGER, year_id INTEGER, semester INTEGER DEFAULT 1, is_approved INTEGER DEFAULT 0, "
               "PRIMARY KEY(section_id, year_id, semester), "
               "FOREIGN KEY(section_id) REFERENCES sections(id), FOREIGN KEY(year_id) REFERENCES academic_years(id))");
    query.exec("ALTER TABLE ranking_approvals ADD COLUMN semester INTEGER DEFAULT 1");

    // --- Data Integrity Migration ---
    // Fix: Ensure marks are associated with the correct year of their section 
    // (Fixes issue where marks were saved to 'latest year' instead of section's year)
    query.exec("UPDATE marks SET year_id = (SELECT year_id FROM sections WHERE sections.id = marks.section_id) "
               "WHERE year_id != (SELECT year_id FROM sections WHERE sections.id = marks.section_id)");
    
    query.exec("UPDATE teaching_assignments SET year_id = (SELECT year_id FROM sections WHERE sections.id = teaching_assignments.section_id) "
               "WHERE year_id != (SELECT year_id FROM sections WHERE sections.id = teaching_assignments.section_id)");

    // --- Seeding Initial Data ---
    
    // Seed Academic Year (Ethiopian Calendar standard)
    query.exec("DELETE FROM academic_years WHERE name LIKE '%/%'"); // Remove old formats like '2015/26'
    query.exec("INSERT OR IGNORE INTO academic_years (name) VALUES ('2018')");
    query.exec("INSERT OR IGNORE INTO academic_years (name) VALUES ('2019')");
    
    // Seed Grade Levels
    QStringList grades = {"9", "10", "11", "12"};
    for (const QString &g : grades) {
        query.prepare("INSERT OR IGNORE INTO grade_levels (name) VALUES (?)");
        query.addBindValue(g);
        query.exec();
    }

    // Seed Streams
    query.exec("INSERT OR IGNORE INTO streams (name) VALUES ('Natural Science')");
    query.exec("INSERT OR IGNORE INTO streams (name) VALUES ('Social Science')");

    // Seed default admin
    QSqlQuery checkAdmin("SELECT * FROM users WHERE username = 'admin'");
    if (!checkAdmin.next()) {
        query.exec("INSERT INTO users (username, password, role, relatedId) "
                   "VALUES ('admin', 'admin123', 'admin', -1)");
    }
               
    return true;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!setupDatabase()) {
        return 1;
    }

    // --- Global Dark Theme Stylesheet ---
    app.setStyleSheet(
        "QWidget {"
        "  background-color: #1a1a2e;"
        "  color: #eaeaea;"
        "  font-family: 'Segoe UI', Arial, sans-serif;"
        "  font-size: 14px;"
        "}"
        "QMainWindow { background-color: #1a1a2e; }"

        /* Sidebar */
        "QWidget#sidebar {"
        "  background-color: #0f3460;"
        "}"

        /* Tables */
        "QTableWidget {"
        "  background-color: #16213e;"
        "  alternate-background-color: #1a1a2e;"
        "  gridline-color: #1f4068;"
        "  border: 1px solid #1f4068;"
        "  border-radius: 8px;"
        "  selection-background-color: #e94560;"
        "  selection-color: #ffffff;"
        "}"
        "QTableWidget::item { padding: 6px; }"
        "QHeaderView::section {"
        "  background-color: #0f3460;"
        "  color: #eaeaea;"
        "  padding: 8px;"
        "  border: none;"
        "  border-bottom: 2px solid #e94560;"
        "  font-weight: bold;"
        "}"

        /* Inputs */
        "QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QDateEdit {"
        "  background-color: #0d1b2a;"
        "  border: 1px solid #1f4068;"
        "  border-radius: 6px;"
        "  padding: 8px 12px;"
        "  color: #eaeaea;"
        "  min-height: 20px;"
        "}"
        "QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus,"
        "QComboBox:focus, QDateEdit:focus {"
        "  border: 1px solid #e94560;"
        "}"

        /* Buttons */
        "QPushButton {"
        "  background-color: #e94560;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 10px 20px;"
        "  font-weight: bold;"
        "  min-width: 80px;"
        "}"
        "QPushButton:hover { background-color: #c81e45; }"
        "QPushButton:pressed { background-color: #a01535; }"
        "QPushButton#secondaryButton { background-color: #1f4068; }"
        "QPushButton#secondaryButton:hover { background-color: #2a5a8c; }"

        /* GroupBox */
        "QGroupBox {"
        "  background-color: #16213e;"
        "  border: 1px solid #1f4068;"
        "  border-radius: 8px;"
        "  margin-top: 12px;"
        "  padding-top: 24px;"
        "  font-weight: bold;"
        "  color: #e94560;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  subcontrol-position: top left;"
        "  padding: 4px 12px;"
        "}"

        /* Labels */
        "QLabel { color: #eaeaea; background-color: transparent; }"
        "QLabel#pageTitle {"
        "  font-size: 24px; font-weight: bold;"
        "  color: #e94560; padding: 10px 0;"
        "}"
        "QLabel#subtitle {"
        "  font-size: 13px; color: #a0a0b0;"
        "}"

        /* Scrollbar */
        "QScrollBar:vertical {"
        "  background: #1a1a2e; width: 8px; border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #1f4068; border-radius: 4px; min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover { background: #e94560; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "}"

        /* MessageBox */
        "QMessageBox { background-color: #16213e; }"
        "QInputDialog { background-color: #16213e; }"
    );

    bool logoutRequested = false;
    do {
        logoutRequested = false;
        LoginDialog loginDlg;
        if (loginDlg.exec() != QDialog::Accepted) {
            return 0;
        }

        MainWindow w(loginDlg.getRole(), loginDlg.getStudentId());
        w.show();
        app.exec();

        if (w.property("logoutRequested").toBool()) {
            logoutRequested = true;
        }
    } while (logoutRequested);

    return 0;
}
