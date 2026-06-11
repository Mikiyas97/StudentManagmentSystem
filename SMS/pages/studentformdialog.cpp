#include "studentformdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>

StudentFormDialog::StudentFormDialog(const Student &s, bool editMode, QWidget *parent)
    : QDialog(parent), editing(editMode)
{
    setWindowTitle(editing ? "Edit Student" : "Add New Student");
    setFixedSize(500, 650); 

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setSpacing(15);
    main->setContentsMargins(20, 20, 20, 20);

    QLabel *title = new QLabel(editing ? "Edit Student" : "Add New Student");
    title->setObjectName("pageTitle");
    main->addWidget(title);

    // --- Academic Info Section ---
    QGroupBox *academicGroup = new QGroupBox("Academic Information");
    QFormLayout *aLayout = new QFormLayout(academicGroup);
    aLayout->setSpacing(12);
    aLayout->setContentsMargins(15, 25, 15, 15);

    idSpin = new QSpinBox;
    idSpin->setRange(1, 9999999);
    if (editing) {
        idSpin->setValue(s.id);
        idSpin->setReadOnly(true);
    } else {
        StudentManager tempMgr;
        idSpin->setValue(tempMgr.generateNextId());
        idSpin->setReadOnly(true);
    }
    aLayout->addRow("Student ID:", idSpin);

    nameEdit = new QLineEdit(s.fullName);
    nameEdit->setPlaceholderText("Full name");
    aLayout->addRow("Full Name:", nameEdit);

    genderCombo = new QComboBox;
    genderCombo->addItems(QStringList() << "Male" << "Female");
    if (!s.gender.isEmpty()) genderCombo->setCurrentText(s.gender);
    aLayout->addRow("Gender:", genderCombo);

    dobEdit = new QLineEdit(s.dateOfBirth);
    dobEdit->setPlaceholderText("DD/MM/YYYY");
    aLayout->addRow("Date of Birth:", dobEdit);

    gradeCombo = new QComboBox;
    auto grades = sectionManager.getAllGrades();
    for (const auto& g : grades) {
        gradeCombo->addItem(g.name, g.id);
    }
    aLayout->addRow("Grade Level:", gradeCombo);

    sectionCombo = new QComboBox;
    auto updateSections = [this](int) {
        sectionCombo->clear();
        int gradeId = gradeCombo->currentData().toInt();
        auto sections = sectionManager.getSectionsByGrade(gradeId);
        for (const auto& s : sections) {
            sectionCombo->addItem(s.name, s.id);
        }
    };
    connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSections);
    aLayout->addRow("Section:", sectionCombo);

    streamCombo = new QComboBox;
    streamCombo->addItem("None", 0);
    auto streams = sectionManager.getAllStreams();
    for (const auto& st : streams) {
        streamCombo->addItem(st.name, st.id);
    }
    
    auto updateStreamLogic = [this](int) {
        QString grade = gradeCombo->currentText();
        bool needsStream = (grade == "11" || grade == "12");
        streamCombo->setEnabled(needsStream);
        if (!needsStream) streamCombo->setCurrentIndex(0); // Select "None"
    };
    connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateStreamLogic);
    aLayout->addRow("Stream (11/12):", streamCombo);

    // Initialize values
    if (s.grade_id > 0) {
        for (int i=0; i<gradeCombo->count(); ++i) {
            if (gradeCombo->itemData(i).toInt() == s.grade_id) {
                gradeCombo->setCurrentIndex(i); break;
            }
        }
    }
    updateSections(gradeCombo->currentIndex());
    if (s.section_id > 0) {
        for (int i=0; i<sectionCombo->count(); ++i) {
            if (sectionCombo->itemData(i).toInt() == s.section_id) {
                sectionCombo->setCurrentIndex(i); break;
            }
        }
    }
    updateStreamLogic(gradeCombo->currentIndex());
    if (s.stream_id > 0) {
        for (int i=0; i<streamCombo->count(); ++i) {
            if (streamCombo->itemData(i).toInt() == s.stream_id) {
                streamCombo->setCurrentIndex(i); break;
            }
        }
    }

    main->addWidget(academicGroup);

    // --- Contact Info Section ---
    QGroupBox *contactGroup = new QGroupBox("Contact Information");
    QFormLayout *cLayout = new QFormLayout(contactGroup);
    cLayout->setSpacing(12);
    cLayout->setContentsMargins(15, 25, 15, 15);

    phoneEdit = new QLineEdit(s.phone);
    phoneEdit->setPlaceholderText("+251...");
    cLayout->addRow("Phone:", phoneEdit);

    emailEdit = new QLineEdit(s.email);
    emailEdit->setPlaceholderText("student@example.com");
    cLayout->addRow("Email:", emailEdit);

    main->addWidget(contactGroup);

    // --- Status Section ---
    if (editing) {
        QGroupBox *statusGroup = new QGroupBox("Status");
        QFormLayout *sLayout = new QFormLayout(statusGroup);
        statusCombo = new QComboBox;
        statusCombo->addItems(QStringList() << "Active" << "Inactive" << "Withdrawn");
        statusCombo->setCurrentText(s.status);
        sLayout->addRow("Status:", statusCombo);
        main->addWidget(statusGroup);
    } else {
        statusCombo = nullptr;
    }

    // --- Action Buttons ---
    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addStretch();

    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("secondaryButton");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton(editing ? "Save Changes" : "Add Student");
    connect(saveBtn, &QPushButton::clicked, this, &StudentFormDialog::onSave);

    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(saveBtn);
    main->addLayout(btnRow);
}

void StudentFormDialog::onSave() {
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Full Name is required.");
        return;
    }

    if (gradeCombo->currentIndex() == -1) {
        QMessageBox::warning(this, "Validation", "Grade Level is required.");
        return;
    }

    result.id             = idSpin->value();
    result.fullName        = nameEdit->text().trimmed();
    result.gender          = genderCombo->currentText();
    result.dateOfBirth     = dobEdit->text().trimmed();
    result.grade_id        = gradeCombo->currentData().toInt();
    result.section_id      = sectionCombo->currentData().toInt();
    result.stream_id       = streamCombo->isEnabled() ? streamCombo->currentData().toInt() : 0;
    result.phone           = phoneEdit->text().trimmed();
    result.email           = emailEdit->text().trimmed();
    result.status          = statusCombo ? statusCombo->currentText() : "Active";

    accept();
}

Student StudentFormDialog::getStudent() const { return result; }