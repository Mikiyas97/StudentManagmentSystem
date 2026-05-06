#include "teacherformdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

TeacherFormDialog::TeacherFormDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Register Teacher");
    setFixedWidth(400);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;

    nameEdit = new QLineEdit;
    
    genderCombo = new QComboBox;
    genderCombo->addItems(QStringList() << "Male" << "Female");
    
    dobEdit = new QLineEdit;
    dobEdit->setPlaceholderText("DD/MM/YYYY");
    
    phoneEdit = new QLineEdit;
    emailEdit = new QLineEdit;
    
    subjectCombo = new QComboBox;
    auto subjects = subjectManager.getAllSubjects();
    for (const auto& sub : subjects) subjectCombo->addItem(sub.name, sub.id);

    form->addRow("Full Name:", nameEdit);
    form->addRow("Gender:", genderCombo);
    form->addRow("Date of Birth:", dobEdit);
    form->addRow("Phone:", phoneEdit);
    form->addRow("Email:", emailEdit);
    form->addRow("Subject Specialization:", subjectCombo);

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
    resultTeacher.gender = genderCombo->currentText();
    resultTeacher.dateOfBirth = dobEdit->text().trimmed();
    resultTeacher.phone = phoneEdit->text().trimmed();
    resultTeacher.email = emailEdit->text().trimmed();
    resultTeacher.subject_id = subjectCombo->currentData().toInt();

    accept();
}

Teacher TeacherFormDialog::getTeacher() const { return resultTeacher; }
