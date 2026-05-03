#include "enrollmentpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>

EnrollmentPage::EnrollmentPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Enrollments");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    // Add Enrollment Group
    QGroupBox *addGroup = new QGroupBox("Manage Enrollment");
    QHBoxLayout *addLayout = new QHBoxLayout(addGroup);
    
    addLayout->addWidget(new QLabel("Student ID:"));
    studentIdSpin = new QSpinBox;
    studentIdSpin->setRange(1, 9999999);
    addLayout->addWidget(studentIdSpin);

    addLayout->addWidget(new QLabel("Course Code:"));
    courseCodeEdit = new QLineEdit;
    addLayout->addWidget(courseCodeEdit);

    QPushButton *enrollBtn = new QPushButton("Enroll");
    QPushButton *unenrollBtn = new QPushButton("Unenroll");
    unenrollBtn->setObjectName("secondaryButton");
    
    addLayout->addWidget(enrollBtn);
    addLayout->addWidget(unenrollBtn);
    addLayout->addStretch();
    layout->addWidget(addGroup);

    connect(enrollBtn, &QPushButton::clicked, this, &EnrollmentPage::onEnroll);
    connect(unenrollBtn, &QPushButton::clicked, this, &EnrollmentPage::onUnenroll);

    // Search
    QGroupBox *searchGroup = new QGroupBox("View Student Enrollments");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchGroup);
    
    searchLayout->addWidget(new QLabel("Student ID:"));
    searchSpin = new QSpinBox;
    searchSpin->setRange(1, 9999999);
    searchLayout->addWidget(searchSpin);

    QPushButton *searchBtn = new QPushButton("Search");
    searchLayout->addWidget(searchBtn);
    searchLayout->addStretch();
    layout->addWidget(searchGroup);

    connect(searchBtn, &QPushButton::clicked, this, &EnrollmentPage::onSearch);

    // Table
    table = new QTableWidget;
    table->setColumnCount(1);
    QStringList headers;
    headers << "Enrolled Course Code";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(table, 1);
}

void EnrollmentPage::onEnroll() {
    int sId = studentIdSpin->value();
    QString code = courseCodeEdit->text().trimmed();
    if (code.isEmpty()) {
        QMessageBox::warning(this, "Error", "Course code is required.");
        return;
    }
    
    // Check if student exists
    if (studentManager.getStudentById(sId).id == -1) {
        QMessageBox::warning(this, "Error", "Student ID does not exist.");
        return;
    }

    if (manager.enrollStudent(sId, code)) {
        QMessageBox::information(this, "Success", "Student enrolled in course.");
        if (searchSpin->value() == sId) refreshTable(sId);
    } else {
        QMessageBox::warning(this, "Error", "Failed to enroll. Maybe already enrolled?");
    }
}

void EnrollmentPage::onUnenroll() {
    int sId = studentIdSpin->value();
    QString code = courseCodeEdit->text().trimmed();
    if (manager.unenrollStudent(sId, code)) {
        QMessageBox::information(this, "Success", "Student unenrolled from course.");
        if (searchSpin->value() == sId) refreshTable(sId);
    } else {
        QMessageBox::warning(this, "Error", "Failed to unenroll.");
    }
}

void EnrollmentPage::onSearch() {
    refreshTable(searchSpin->value());
}

void EnrollmentPage::refreshTable(int studentId) {
    QVector<QString> courses = manager.getCoursesForStudent(studentId);
    table->setRowCount(courses.size());
    for (int i = 0; i < courses.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(courses[i]));
    }
}
