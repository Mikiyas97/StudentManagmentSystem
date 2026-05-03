#include "teacherpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

TeacherPage::TeacherPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Teachers");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    // Add Teacher Group
    QGroupBox *addGroup = new QGroupBox("Add New Teacher");
    QHBoxLayout *addLayout = new QHBoxLayout(addGroup);
    
    nameEdit = new QLineEdit; nameEdit->setPlaceholderText("Full Name");
    courseEdit = new QLineEdit; courseEdit->setPlaceholderText("Course Code");
    phoneEdit = new QLineEdit; phoneEdit->setPlaceholderText("Phone");
    emailEdit = new QLineEdit; emailEdit->setPlaceholderText("Email");
    
    addLayout->addWidget(nameEdit);
    addLayout->addWidget(courseEdit);
    addLayout->addWidget(phoneEdit);
    addLayout->addWidget(emailEdit);
    
    QPushButton *addBtn = new QPushButton("Add");
    addBtn->setCursor(Qt::PointingHandCursor);
    addLayout->addWidget(addBtn);
    layout->addWidget(addGroup);

    connect(addBtn, &QPushButton::clicked, this, &TeacherPage::onAddTeacher);

    // Table
    table = new QTableWidget;
    table->setColumnCount(5);
    QStringList headers;
    headers << "ID" << "Name" << "Course" << "Phone" << "Email";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    layout->addWidget(table, 1);

    refreshTable();
}

void TeacherPage::onAddTeacher() {
    QString name = nameEdit->text().trimmed();
    QString course = courseEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Teacher name is required.");
        return;
    }

    Teacher t;
    t.id = manager.generateNextTeacherId();
    QString pass = "pass" + QString::number(t.id); // Default password: pass<id>
    
    t.fullName = name;
    t.courseCode = course;
    t.phone = phone;
    t.email = email;

    if (manager.addTeacher(t, pass)) {
        QMessageBox::information(this, "Success", "Teacher added!\nUsername: " + QString::number(t.id) + "\nPassword: " + pass);
        nameEdit->clear(); courseEdit->clear(); phoneEdit->clear(); emailEdit->clear();
        refreshTable();
    } else {
        QSqlQuery lastQ;
        lastQ.exec("SELECT 1"); // dummy to get a query object if needed, but better use manager error
        QMessageBox::warning(this, "Error", "Failed to add teacher.\nThis could be due to a duplicate ID or database schema mismatch.");
    }
}

void TeacherPage::refreshTable() {
    QVector<Teacher> teachers = manager.getTeachers();
    table->setRowCount(teachers.size());
    for (int i = 0; i < teachers.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(teachers[i].id)));
        table->setItem(i, 1, new QTableWidgetItem(teachers[i].fullName));
        table->setItem(i, 2, new QTableWidgetItem(teachers[i].courseCode));
        table->setItem(i, 3, new QTableWidgetItem(teachers[i].phone));
        table->setItem(i, 4, new QTableWidgetItem(teachers[i].email));
    }
}
