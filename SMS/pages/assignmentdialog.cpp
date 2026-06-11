#include "assignmentdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
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
    auto grades = sectionManager.getAllGrades();
    for (const auto& g : grades) gradeCombo->addItem("Grade " + g.name, g.id);

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
    auto sections = sectionManager.getSectionsByGrade(gradeId);
    for (const auto& s : sections) sectionCombo->addItem(s.name, s.id);
    onSectionChanged(); // Update subjects based on first section
}

void AssignmentDialog::onSectionChanged() {
    subjectCombo->clear();
    int sectionId = sectionCombo->currentData().toInt();
    
    // Get subjects for this section's grade
    auto subjects = subManager.getSubjectsForSection(sectionId);
    for (const auto& sub : subjects) subjectCombo->addItem(sub.name, sub.id);
}

void AssignmentDialog::onSubjectChanged() {
    teacherCombo->clear();
    int subjectId = subjectCombo->currentData().toInt();
    
    // Rule 1: Only show teachers who specialize in this subject
    auto teachers = userManager.getTeachersBySubject(subjectId);
    for (const auto& t : teachers) teacherCombo->addItem(t.fullName, t.id);
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
