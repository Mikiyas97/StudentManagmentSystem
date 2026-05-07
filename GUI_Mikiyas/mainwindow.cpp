#include "mainwindow.h"
#include "pages/studentpage.h"
#include "pages/gradepage.h"
#include "pages/coursepage.h"
#include "pages/reportpage.h"
#include "pages/teacherpage.h"
#include "pages/enrollmentpage.h"
#include "pages/offeringpage.h"
#include "pages/profilepage.h"
#include "managers/usermanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(const QString &role, int studentId, QWidget *parent)
    : QMainWindow(parent), userRole(role), userStudentId(studentId), activePage(-1)
{
    setWindowTitle("School Management System");
    resize(1100, 700);
    setupUI();
}

QPushButton* MainWindow::makeNavButton(const QString &text, int index) {
    QPushButton *btn = new QPushButton(text);
    btn->setObjectName("navButton");
    btn->setCursor(Qt::PointingHandCursor);
    btn->setCheckable(true);
    connect(btn, &QPushButton::clicked, this, [this, index]() { switchPage(index); });
    navButtons.push_back(btn);
    return btn;
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget;
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Sidebar ---
    QWidget *sidebar = new QWidget;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(230);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Sidebar header
    QLabel *logo = new QLabel(QString::fromUtf8("  \xF0\x9F\x8E\x93 SMS"));
    logo->setObjectName("sidebarLogo");
    logo->setStyleSheet(
        "#sidebarLogo { font-size: 22px; font-weight: bold; color: #e94560;"
        "padding: 20px 15px 10px 15px; background: transparent; }"
    );
    sideLayout->addWidget(logo);

    QLabel *divider = new QLabel;
    divider->setFixedHeight(1);
    divider->setStyleSheet("background-color: #1f4068; margin: 5px 15px;");
    sideLayout->addWidget(divider);
    sideLayout->addSpacing(5);

    // Nav buttons based on role
    if (userRole == "admin") {
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x93\x8A  Dashboard"), 0));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x8E\x93  Students"), 1));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x91\xA4  Teachers"), 2));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x93\x9A  Subjects"), 3));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x8F\xAB  Sections"), 4));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x93\x9D  Marks"), 5));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x8F\x86  Ranking"), 6));
    } else if (userRole == "teacher") {
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x91\xA4  My Profile"), 0));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x8E\x93  My Students"), 1));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x93\x9D  Enter Marks"), 2));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x8F\x86  Class Ranking"), 3));
    } else {
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x91\xA4  My Profile"), 0));
        sideLayout->addWidget(makeNavButton(QString::fromUtf8("  \xF0\x9F\x93\x9D  My Marks"), 1));
    }

    sideLayout->addStretch();

    // Role info at bottom of sidebar
    roleLabel = new QLabel;
    QString roleText = "  Role: " + userRole;
    if (userRole == "student" || userRole == "teacher") roleText += "\n  ID: " + QString::number(userStudentId);
    roleLabel->setText(roleText);
    roleLabel->setStyleSheet(
        "color: #a0a0b0; font-size: 12px; padding: 15px;"
        "background: transparent; border-top: 1px solid #1f4068;"
    );
    sideLayout->addWidget(roleLabel);

    if (userRole != "admin") {
        QPushButton *changePassBtn = new QPushButton(QString::fromUtf8("  \xF0\x9F\x94\x92  Change Pass"));
        changePassBtn->setObjectName("navButton");
        changePassBtn->setCursor(Qt::PointingHandCursor);
        changePassBtn->setStyleSheet(
            "QPushButton { color: #3498db; text-align: left; padding: 6px 20px;"
            "border: none; background: transparent; font-size: 13px; }"
            "QPushButton:hover { background-color: #162447; }"
        );
        connect(changePassBtn, &QPushButton::clicked, this, [this]() {
            bool ok;
            QString newPass = QInputDialog::getText(this, "Change Password", "Enter new password:", QLineEdit::Password, "", &ok);
            if (ok && !newPass.isEmpty()) {
                UserManager um;
                um.changePassword(QString::number(userStudentId), newPass);
                QMessageBox::information(this, "Success", "Password changed successfully.");
            }
        });
        sideLayout->addWidget(changePassBtn);
    }

    // Logout button
    QPushButton *logoutBtn = new QPushButton(QString::fromUtf8("  \xF0\x9F\x9A\xAA  Logout"));
    logoutBtn->setObjectName("navButton");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton { color: #e74c3c; text-align: left; padding: 12px 20px;"
        "border: none; background: transparent; font-size: 14px; }"
        "QPushButton:hover { background-color: #2c1a1a; }"
    );
    connect(logoutBtn, &QPushButton::clicked, this, [this]() {
        this->setProperty("logoutRequested", true);
        this->close();
    });
    sideLayout->addWidget(logoutBtn);

    mainLayout->addWidget(sidebar);

    // --- Content area ---
    QWidget *contentWrapper = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWrapper);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Top Header for global actions
    QWidget *header = new QWidget;
    header->setFixedHeight(50);
    header->setStyleSheet("background-color: #1a1a2e; border-bottom: 1px solid #16213e;");
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 0, 20, 0);
    headerLayout->addStretch();

    QPushButton *refreshBtn = new QPushButton(QString::fromUtf8("\xE2\x9A\xA1 Sync"));
    refreshBtn->setObjectName("syncButton");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setToolTip("Synchronize and Refresh Page");
    refreshBtn->setStyleSheet(
        "QPushButton { background-color: #e94560; color: white; padding: 6px 15px; "
        "border-radius: 15px; font-weight: bold; font-size: 13px; border: none; }"
        "QPushButton:hover { background-color: #ff5e78; }"
    );
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onGlobalRefresh);
    headerLayout->addWidget(refreshBtn);

    contentLayout->addWidget(header);

    stack = new QStackedWidget;
    stack->setObjectName("contentArea");

    if (userRole == "admin") {
        stack->addWidget(new ReportPage);              // 0
        stack->addWidget(new StudentPage("admin"));    // 1
        stack->addWidget(new TeacherPage);             // 2
        stack->addWidget(new CoursePage);              // 3
        stack->addWidget(new OfferingPage);            // 4
        stack->addWidget(new EnrollmentPage("admin")); // 5
        stack->addWidget(new GradePage("admin"));      // 6
    } else if (userRole == "teacher") {
        stack->addWidget(new ProfilePage("teacher", userStudentId)); // 0
        stack->addWidget(new StudentPage("teacher", userStudentId)); // 1
        stack->addWidget(new EnrollmentPage("teacher", userStudentId)); // 2 (MarkEntry)
        stack->addWidget(new GradePage("teacher", userStudentId));   // 3 (Ranking)
    } else {
        stack->addWidget(new ProfilePage("student", userStudentId)); // 0
        stack->addWidget(new EnrollmentPage("student", userStudentId)); // 1 (View Marks)
    }

    contentLayout->addWidget(stack);
    mainLayout->addWidget(contentWrapper, 1);

    // Select first page
    switchPage(0);
}

void MainWindow::switchPage(int index) {
    if (index < 0 || index >= stack->count()) return;
    stack->setCurrentIndex(index);
    activePage = index;
    
    // Auto-refresh when switching pages
    onGlobalRefresh();

    // Update active button styling
    for (int i = 0; i < navButtons.size(); ++i) {
        navButtons[i]->setChecked(i == index);
        if (i == index) {
            navButtons[i]->setStyleSheet(
                "QPushButton { background-color: #162447; color: #e94560;"
                "text-align: left; padding: 12px 20px; border: none;"
                "border-left: 3px solid #e94560; font-size: 14px; font-weight: bold; }"
            );
        } else {
            navButtons[i]->setStyleSheet(
                "QPushButton { background-color: transparent; color: #a0a0b0;"
                "text-align: left; padding: 12px 20px; border: none;"
                "border-left: 3px solid transparent; font-size: 14px; }"
                "QPushButton:hover { background-color: #162447; color: #eaeaea; }"
            );
        }
    }
    activePage = index;
}

void MainWindow::onGlobalRefresh() {
    QWidget *currentPage = stack->currentWidget();
    if (currentPage) {
        // Dynamically invoke refreshTable() if it exists on the page
        QMetaObject::invokeMethod(currentPage, "refreshTable");
    }
}
