#include "teacherpage.h"
#include "teacherformdialog.h"
#include "teacherdetaildialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include "../managers/subjectmanager.h"

TeacherPage::TeacherPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    // --- Header ---
    QHBoxLayout *headerLayout = new QHBoxLayout;
    // --- Toolbar ---
    QHBoxLayout *toolbar = new QHBoxLayout;
    
    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Search by Name, Phone, or Email...");
    searchEdit->setFixedWidth(300);
    searchEdit->setStyleSheet("padding: 8px; border-radius: 4px; background: #16213e; color: white; border: 1px solid #1f4068;");
    connect(searchEdit, &QLineEdit::textChanged, this, &TeacherPage::refreshTable);
    
    toolbar->addWidget(new QLabel("🔍"));
    toolbar->addWidget(searchEdit);
    toolbar->addSpacing(20);
    
    toolbar->addWidget(new QLabel("Sort By:"));
    sortCombo = new QComboBox;
    sortCombo->addItem("ID Asc", "id ASC");
    sortCombo->addItem("ID Desc", "id DESC");
    sortCombo->addItem("Name A-Z", "fullName ASC");
    sortCombo->addItem("Name Z-A", "fullName DESC");
    sortCombo->setStyleSheet("padding: 5px; border-radius: 4px; background: #16213e; color: white;");
    connect(sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TeacherPage::onSortChanged);
    toolbar->addWidget(sortCombo);
    
    toolbar->addStretch();
    
    QPushButton *addBtn = new QPushButton("Register New Teacher");
    addBtn->setStyleSheet("background-color: #e94560; color: white; padding: 8px 15px; font-weight: bold; border-radius: 4px;");
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &TeacherPage::onAddTeacher);
    toolbar->addWidget(addBtn);
    
    layout->addLayout(toolbar);

    // --- Table ---
    table = new QTableWidget;
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Full Name", "Phone", "Email"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    connect(table, &QTableWidget::cellDoubleClicked, this, &TeacherPage::onRowDoubleClicked);
    
    layout->addWidget(table, 1);

    refreshTable();
}

void TeacherPage::onAddTeacher() {
    TeacherFormDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Teacher t = dialog.getTeacher();
        t.id = manager.generateNextTeacherId();
        QString pass = "pass" + QString::number(t.id);

        if (manager.addTeacher(t, pass)) {
            // Handle Assignment
            SubjectManager sm;
            sm.assignTeacherToSubject(t.id, dialog.getSubjectId(), dialog.getSectionId(), 1);

            QMessageBox::information(this, "Success", "Teacher Registered!\nID: " + QString::number(t.id) + "\nPassword: " + pass);
            refreshTable();
        } else {
            QMessageBox::warning(this, "Error", "Failed to add teacher.");
        }
    }
}

void TeacherPage::onRowDoubleClicked(int row, int column) {
    int id = table->item(row, 0)->text().toInt();
    Teacher t = manager.getTeacherById(id);
    if (t.id != -1) {
        TeacherDetailDialog dialog(t, this);
        dialog.exec();
    }
}

void TeacherPage::onSortChanged(int /*index*/) {
    refreshTable();
}

void TeacherPage::refreshTable() {
    QString sortBy = sortCombo ? sortCombo->currentData().toString() : "";
    QVector<Teacher> teachers = manager.filterTeachers(searchEdit->text().trimmed(), sortBy);
    table->setRowCount(teachers.size());
    for (int i = 0; i < teachers.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(teachers[i].id)));
        table->setItem(i, 1, new QTableWidgetItem(teachers[i].fullName));
        table->setItem(i, 2, new QTableWidgetItem(teachers[i].phone));
        table->setItem(i, 3, new QTableWidgetItem(teachers[i].email));
    }
}
