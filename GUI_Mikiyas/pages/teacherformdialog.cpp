#include "teacherformdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QSqlQuery>
#include <QMessageBox>

TeacherFormDialog::TeacherFormDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Register Teacher");
    setFixedWidth(400);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;

    nameEdit = new QLineEdit;
    phoneEdit = new QLineEdit;
    emailEdit = new QLineEdit;
    
    subjectCombo = new QComboBox;
    QSqlQuery sq("SELECT id, name FROM subjects");
    while (sq.next()) subjectCombo->addItem(sq.value("name").toString(), sq.value("id").toInt());
    
    sectionCombo = new QComboBox;
    QSqlQuery secq("SELECT id, name FROM sections");
    while (secq.next()) sectionCombo->addItem(secq.value("name").toString(), secq.value("id").toInt());

    form->addRow("Full Name:", nameEdit);
    form->addRow("Phone:", phoneEdit);
    form->addRow("Email:", emailEdit);
    form->addRow("Assign Subject:", subjectCombo);
    form->addRow("Assign Section:", sectionCombo);

    layout->addLayout(form);

    QPushButton *saveBtn = new QPushButton("Register");
    saveBtn->setStyleSheet("background-color: #e94560; color: white; padding: 10px; font-weight: bold;");
    connect(saveBtn, &QPushButton::clicked, this, &TeacherFormDialog::onSave);
    layout->addWidget(saveBtn);
}

void TeacherFormDialog::onSave() {
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Name is required.");
        return;
    }

    resultTeacher.fullName = nameEdit->text().trimmed();
    resultTeacher.phone = phoneEdit->text().trimmed();
    resultTeacher.email = emailEdit->text().trimmed();
    resultSubjectId = subjectCombo->currentData().toInt();
    resultSectionId = sectionCombo->currentData().toInt();

    accept();
}

Teacher TeacherFormDialog::getTeacher() const { return resultTeacher; }
int TeacherFormDialog::getSubjectId() const { return resultSubjectId; }
int TeacherFormDialog::getSectionId() const { return resultSectionId; }
