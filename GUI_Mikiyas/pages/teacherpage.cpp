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
#include "../managers/subjectmanager.h"

TeacherPage::TeacherPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Teachers Management");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    // Add Teacher Group
    QGroupBox *addGroup = new QGroupBox("Add New Teacher & Assign Subject");
    QHBoxLayout *addLayout = new QHBoxLayout(addGroup);
    
    nameEdit = new QLineEdit; nameEdit->setPlaceholderText("Full Name");
    
    subjectCombo = new QComboBox;
    QSqlQuery sq("SELECT id, name FROM subjects");
    while (sq.next()) subjectCombo->addItem(sq.value("name").toString(), sq.value("id").toInt());
    
    sectionCombo = new QComboBox;
    QSqlQuery secq("SELECT id, name FROM sections");
    while (secq.next()) sectionCombo->addItem(secq.value("name").toString(), secq.value("id").toInt());

    phoneEdit = new QLineEdit; phoneEdit->setPlaceholderText("Phone");
    emailEdit = new QLineEdit; emailEdit->setPlaceholderText("Email");
    
    addLayout->addWidget(nameEdit);
    addLayout->addWidget(new QLabel("Subject:"));
    addLayout->addWidget(subjectCombo);
    addLayout->addWidget(new QLabel("Section:"));
    addLayout->addWidget(sectionCombo);
    addLayout->addWidget(phoneEdit);
    addLayout->addWidget(emailEdit);
    
    QPushButton *addBtn = new QPushButton("Register Teacher");
    addBtn->setCursor(Qt::PointingHandCursor);
    addLayout->addWidget(addBtn);
    layout->addWidget(addGroup);

    connect(addBtn, &QPushButton::clicked, this, &TeacherPage::onAddTeacher);

    // Table
    table = new QTableWidget;
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Full Name", "Phone", "Email"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    layout->addWidget(table, 1);

    refreshTable();
}

void TeacherPage::onAddTeacher() {
    QString name = nameEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Teacher name is required.");
        return;
    }

    Teacher t;
    t.id = manager.generateNextTeacherId();
    QString pass = "pass" + QString::number(t.id); // Corrected password pattern
    
    t.fullName = name;
    t.phone = phone;
    t.email = email;

    if (manager.addTeacher(t, pass)) {
        // Handle Assignment
        int subId = subjectCombo->currentData().toInt();
        int secId = sectionCombo->currentData().toInt();
        SubjectManager sm;
        sm.assignTeacherToSubject(t.id, subId, secId, 1); // Using default year ID 1

        QMessageBox::information(this, "Success", "Teacher Registered!\nID: " + QString::number(t.id) + "\nPassword: " + pass);
        nameEdit->clear(); phoneEdit->clear(); emailEdit->clear();
        refreshTable();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add teacher. Check database connection.");
    }
}

void TeacherPage::refreshTable() {
    QVector<Teacher> teachers = manager.getTeachers();
    table->setRowCount(teachers.size());
    for (int i = 0; i < teachers.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(teachers[i].id)));
        table->setItem(i, 1, new QTableWidgetItem(teachers[i].fullName));
        table->setItem(i, 2, new QTableWidgetItem(teachers[i].phone));
        table->setItem(i, 3, new QTableWidgetItem(teachers[i].email));
    }
}
