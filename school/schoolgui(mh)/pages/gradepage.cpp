#include "gradepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>

GradePage::GradePage(const QString &role, int studentId, QWidget *parent)
    : QWidget(parent), userRole(role), userStudentId(studentId)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Grades");
    title->setObjectName("pageTitle");
    QLabel *subtitle = new QLabel("Manage and view student grades");
    subtitle->setObjectName("subtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Add Grade form (admin only)
    if (userRole == "admin") {
        QGroupBox *addGroup = new QGroupBox("Add New Grade");
        QHBoxLayout *addLayout = new QHBoxLayout(addGroup);
        addLayout->setSpacing(10);

        addLayout->addWidget(new QLabel("Student ID:"));
        sidSpin = new QSpinBox;
        sidSpin->setRange(1, 9999999);
        sidSpin->setFixedWidth(120);
        addLayout->addWidget(sidSpin);

        addLayout->addWidget(new QLabel("Course:"));
        courseEdit = new QLineEdit;
        courseEdit->setPlaceholderText("Course name");
        courseEdit->setFixedWidth(150);
        addLayout->addWidget(courseEdit);

        addLayout->addWidget(new QLabel("Score:"));
        scoreSpin = new QDoubleSpinBox;
        scoreSpin->setRange(0, 100);
        scoreSpin->setDecimals(1);
        scoreSpin->setFixedWidth(100);
        addLayout->addWidget(scoreSpin);

        QPushButton *addBtn = new QPushButton("Add Grade");
        addBtn->setCursor(Qt::PointingHandCursor);
        addLayout->addWidget(addBtn);
        addLayout->addStretch();
        layout->addWidget(addGroup);

        connect(addBtn, &QPushButton::clicked, this, &GradePage::onAdd);
    }

    // Filter bar (admin only)
    if (userRole == "admin") {
        QGroupBox *filterGroup = new QGroupBox("Filter");
        QHBoxLayout *filterLayout = new QHBoxLayout(filterGroup);
        filterLayout->setSpacing(10);

        filterLayout->addWidget(new QLabel("Student ID:"));
        filterSpin = new QSpinBox;
        filterSpin->setRange(0, 9999999);
        filterSpin->setSpecialValueText("All");
        filterSpin->setFixedWidth(120);
        filterLayout->addWidget(filterSpin);

        QPushButton *filterBtn = new QPushButton("Filter");
        filterBtn->setObjectName("secondaryButton");
        filterBtn->setCursor(Qt::PointingHandCursor);
        filterLayout->addWidget(filterBtn);

        QPushButton *refreshBtn = new QPushButton("Show All");
        refreshBtn->setObjectName("secondaryButton");
        refreshBtn->setCursor(Qt::PointingHandCursor);
        filterLayout->addWidget(refreshBtn);
        filterLayout->addStretch();
        layout->addWidget(filterGroup);

        connect(filterBtn, &QPushButton::clicked, this, &GradePage::onFilter);
        connect(refreshBtn, &QPushButton::clicked, this, [this]() { refreshTable(-1); });
    }

    // Table
    table = new QTableWidget;
    table->setColumnCount(3);
    QStringList headers;
    headers << "Student ID" << "Course" << "Score";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);
    layout->addWidget(table, 1);

    // Initial load
    if (userRole == "student") {
        refreshTable(userStudentId);
    } else {
        refreshTable(-1);
    }
}

void GradePage::refreshTable(int filterId) {
    manager = GradeManager();
    QVector<Grade> grades = manager.getGrades(filterId);
    table->setRowCount(grades.size());
    for (int i = 0; i < grades.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(grades[i].studentId)));
        table->setItem(i, 1, new QTableWidgetItem(grades[i].course));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(grades[i].score, 'f', 1)));
    }
}

void GradePage::onAdd() {
    QString course = courseEdit->text().trimmed();
    if (course.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a course name.");
        return;
    }
    manager.addGrade(sidSpin->value(), course, scoreSpin->value());
    QMessageBox::information(this, "Success", "Grade recorded!");
    courseEdit->clear();
    refreshTable(-1);
}

void GradePage::onFilter() {
    int id = filterSpin->value();
    refreshTable(id == 0 ? -1 : id);
}
