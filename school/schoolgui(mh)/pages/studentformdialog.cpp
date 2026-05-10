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
    // Increased height slightly to prevent vertical compression
    setFixedSize(520, 720); 

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setSpacing(15);
    main->setContentsMargins(20, 20, 20, 20);

    QLabel *title = new QLabel(editing ? "Edit Student" : "Add New Student");
    title->setObjectName("pageTitle");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #e94560; margin-bottom: 5px;");
    main->addWidget(title);

    // --- Personal Info Section ---
    QGroupBox *personalGroup = new QGroupBox("Personal Information");
    QFormLayout *pLayout = new QFormLayout(personalGroup);
    pLayout->setSpacing(12);
    pLayout->setContentsMargins(15, 25, 15, 15);
    pLayout->setLabelAlignment(Qt::AlignLeft);

    idSpin = new QSpinBox;
    idSpin->setRange(1, 9999999);
    idSpin->setValue(s.id > 0 ? s.id : 1);
    if (editing) idSpin->setReadOnly(true);
    pLayout->addRow("Student ID:", idSpin);

    nameEdit = new QLineEdit(s.fullName);
    nameEdit->setPlaceholderText("Full name");
    pLayout->addRow("Full Name:", nameEdit);

    ageSpin = new QSpinBox;
    ageSpin->setRange(3, 100);
    ageSpin->setValue(s.age > 0 ? s.age : 18);
    pLayout->addRow("Age:", ageSpin);

    genderCombo = new QComboBox;
    genderCombo->addItems(QStringList() << "Male" << "Female" << "Other");
    if (!s.gender.isEmpty()) genderCombo->setCurrentText(s.gender);
    pLayout->addRow("Gender:", genderCombo);

    classEdit = new QLineEdit(s.className);
    classEdit->setPlaceholderText("e.g. Grade 10, CS Department");
    pLayout->addRow("Class / Dept:", classEdit);

    main->addWidget(personalGroup);

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

    addressEdit = new QTextEdit(s.address);
    addressEdit->setPlaceholderText("Full address");
    addressEdit->setMaximumHeight(60);
    cLayout->addRow("Address:", addressEdit);

    main->addWidget(contactGroup);

    // --- Guardian Info Section ---
    QGroupBox *guardGroup = new QGroupBox("Guardian Information");
    QFormLayout *gLayout = new QFormLayout(guardGroup);
    gLayout->setSpacing(12);
    gLayout->setContentsMargins(15, 25, 15, 15);

    guardianEdit = new QLineEdit(s.guardianName);
    guardianEdit->setPlaceholderText("Parent/Guardian name");
    gLayout->addRow("Guardian:", guardianEdit);

    guardianPhoneEdit = new QLineEdit(s.guardianContact);
    guardianPhoneEdit->setPlaceholderText("Guardian phone");
    gLayout->addRow("Guardian Phone:", guardianPhoneEdit);

    main->addWidget(guardGroup);

    // --- Status Section (Edit Mode Only) ---
    if (editing) {
        QGroupBox *statusGroup = new QGroupBox("Status");
        QFormLayout *sLayout = new QFormLayout(statusGroup);
        sLayout->setContentsMargins(15, 25, 15, 15);
        
        statusCombo = new QComboBox;
        statusCombo->addItems(QStringList() << "Active" << "Inactive" << "Graduated");
        statusCombo->setCurrentText(s.status);
        sLayout->addRow("Status:", statusCombo);
        main->addWidget(statusGroup);
    } else {
        statusCombo = nullptr;
    }

    // --- Action Buttons ---
    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setContentsMargins(0, 10, 0, 0);
    btnRow->addStretch();

    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("secondaryButton");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setMinimumSize(100, 35);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton(editing ? "Save Changes" : "Add Student");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setMinimumSize(120, 35);
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
    if (classEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Class / Department is required.");
        return;
    }

    result.id             = idSpin->value();
    result.fullName        = nameEdit->text().trimmed();
    result.age             = ageSpin->value();
    result.gender          = genderCombo->currentText();
    result.className       = classEdit->text().trimmed();
    result.phone           = phoneEdit->text().trimmed();
    result.email           = emailEdit->text().trimmed();
    // Clean data for file safety
    result.address         = addressEdit->toPlainText().trimmed().replace('|', ' ').replace('\n', ' ');
    result.guardianName    = guardianEdit->text().trimmed();
    result.guardianContact = guardianPhoneEdit->text().trimmed();
    result.status          = statusCombo ? statusCombo->currentText() : "Active";

    accept();
}

Student StudentFormDialog::getStudent() const { return result; }