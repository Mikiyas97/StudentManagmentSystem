#include "coursepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>

CoursePage::CoursePage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Courses");
    title->setObjectName("pageTitle");
    QLabel *subtitle = new QLabel("Manage course catalog");
    subtitle->setObjectName("subtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    QGroupBox *addGroup = new QGroupBox("Add New Course");
    QHBoxLayout *addLayout = new QHBoxLayout(addGroup);
    addLayout->setSpacing(10);

    addLayout->addWidget(new QLabel("Course ID:"));
    idEdit = new QLineEdit;
    idEdit->setPlaceholderText("e.g. CS101");
    idEdit->setFixedWidth(120);
    addLayout->addWidget(idEdit);

    addLayout->addWidget(new QLabel("Name:"));
    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Course name");
    nameEdit->setFixedWidth(180);
    addLayout->addWidget(nameEdit);

    addLayout->addWidget(new QLabel("Credits:"));
    creditsSpin = new QSpinBox;
    creditsSpin->setRange(1, 10);
    creditsSpin->setFixedWidth(80);
    addLayout->addWidget(creditsSpin);

    QPushButton *addBtn = new QPushButton("Add Course");
    addBtn->setCursor(Qt::PointingHandCursor);
    addLayout->addWidget(addBtn);
    addLayout->addStretch();
    layout->addWidget(addGroup);

    table = new QTableWidget;
    table->setColumnCount(3);
    QStringList headers;
    headers << "Course ID" << "Name" << "Credits";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);
    layout->addWidget(table, 1);

    connect(addBtn, &QPushButton::clicked, this, &CoursePage::onAdd);
    refreshTable();
}

void CoursePage::refreshTable() {
    manager = CourseManager();
    QVector<Course> courses = manager.getCourses();
    table->setRowCount(courses.size());
    for (int i = 0; i < courses.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(courses[i].id));
        table->setItem(i, 1, new QTableWidgetItem(courses[i].name));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(courses[i].credits)));
    }
}

void CoursePage::onAdd() {
    QString id = idEdit->text().trimmed();
    QString name = nameEdit->text().trimmed();
    if (id.isEmpty() || name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }
    if (manager.addCourse(id, name, creditsSpin->value())) {
        QMessageBox::information(this, "Success", "Course added!");
        idEdit->clear();
        nameEdit->clear();
        refreshTable();
    } else {
        QMessageBox::warning(this, "Error", "Course ID already exists.");
    }
}
