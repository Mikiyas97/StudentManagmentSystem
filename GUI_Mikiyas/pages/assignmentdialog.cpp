#include "assignmentdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QSqlQuery>
#include <QMessageBox>
#include <QLabel>

AssignmentDialog::AssignmentDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Teacher Assignment");
    setFixedWidth(450);
    setStyleSheet("QDialog { background-color: #1a1a2e; } QLabel { color: #eaeaea; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(25, 25, 25, 25);

    QLabel *header = new QLabel("Create Teaching Assignment");
    header->setStyleSheet("font-size: 18px; font-weight: bold; color: #e94560; margin-bottom: 10px;");
    layout->addWidget(header);

    QFormLayout *form = new QFormLayout;
    form->setSpacing(15);

    gradeCombo = new QComboBox;
    QSqlQuery gq("SELECT id, name FROM grade_levels ORDER BY CAST(name AS INTEGER) ASC");
    while (gq.next()) gradeCombo->addItem("Grade " + gq.value("name").toString(), gq.value("id").toInt());

    sectionCombo = new QComboBox;
    subjectCombo = new QComboBox;
    teacherCombo = new QComboBox;

    QString comboStyle = "QComboBox { padding: 8px; border-radius: 4px; background: #16213e; color: white; border: 1px solid #1f4068; }";
    gradeCombo->setStyleSheet(comboStyle);
    sectionCombo->setStyleSheet(comboStyle);
    subjectCombo->setStyleSheet(comboStyle);
    teacherCombo->setStyleSheet(comboStyle);

    form->addRow("Select Grade:", gradeCombo);
    form->addRow("Select Section:", sectionCombo);
    form->addRow("Select Subject:", subjectCombo);
    form->addRow("Assign Teacher:", teacherCombo);

    layout->addLayout(form);

    QPushButton *assignBtn = new QPushButton("Create Assignment");
    assignBtn->setCursor(Qt::PointingHandCursor);
    assignBtn->setStyleSheet("QPushButton { background-color: #2ecc71; color: white; padding: 12px; font-weight: bold; border-radius: 6px; font-size: 14px; }"
                             "QPushButton:hover { background-color: #27ae60; }");
    connect(assignBtn, &QPushButton::clicked, this, &AssignmentDialog::onAssign);
    layout->addWidget(assignBtn);

    connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AssignmentDialog::onGradeChanged);
    connect(sectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AssignmentDialog::onSectionChanged);
    connect(subjectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AssignmentDialog::onSubjectChanged);

    onGradeChanged(); // Initial load
}

void AssignmentDialog::onGradeChanged() {
    sectionCombo->clear();
    int gradeId = gradeCombo->currentData().toInt();
    QSqlQuery q;
    q.prepare("SELECT id, name FROM sections WHERE grade_id = ?");
    q.addBindValue(gradeId);
    if (q.exec()) {
        while (q.next()) sectionCombo->addItem(q.value("name").toString(), q.value("id").toInt());
    }
    onSectionChanged(); // Update subjects based on first section
}

void AssignmentDialog::onSectionChanged() {
    subjectCombo->clear();
    int sectionId = sectionCombo->currentData().toInt();
    
    // Get subjects for this section's grade
    QSqlQuery q;
    q.prepare("SELECT sub.id, sub.name FROM subjects sub "
              "JOIN sections sec ON sub.grade_id = sec.grade_id "
              "WHERE sec.id = ?");
    q.addBindValue(sectionId);
    if (q.exec()) {
        while (q.next()) subjectCombo->addItem(q.value("name").toString(), q.value("id").toInt());
    }
}

void AssignmentDialog::onSubjectChanged() {
    teacherCombo->clear();
    int subjectId = subjectCombo->currentData().toInt();
    
    // Rule 1: Only show teachers who specialize in this subject
    QSqlQuery q;
    q.prepare("SELECT id, fullName FROM teachers WHERE subject_id = ?");
    q.addBindValue(subjectId);
    if (q.exec()) {
        while (q.next()) teacherCombo->addItem(q.value("fullName").toString(), q.value("id").toInt());
    }
}

void AssignmentDialog::onAssign() {
    int sectionId = sectionCombo->currentData().toInt();
    int subjectId = subjectCombo->currentData().toInt();
    int teacherId = teacherCombo->currentData().toInt();

    if (teacherId <= 0) {
        QMessageBox::warning(this, "Error", "No teacher selected.");
        return;
    }

    if (subManager.assignTeacherToSubject(teacherId, subjectId, sectionId, 1)) {
        QMessageBox::information(this, "Success", "Teacher assigned successfully!");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Failed to assign teacher. This subject might already have a teacher in this section.");
    }
}
