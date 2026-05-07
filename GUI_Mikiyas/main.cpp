#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "logindialog.h"
#include "mainwindow.h"
#include "managers/databasemanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    if (!DatabaseManager::initDatabase()) {
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
