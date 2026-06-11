#include "teacherdetaildialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>
#include "../managers/subjectmanager.h"
#include "../managers/usermanager.h"

TeacherDetailDialog::TeacherDetailDialog(const Teacher &t, const QString &userRole, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Teacher Information");
    setFixedWidth(350);

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *header = new QLabel(t.fullName);
    header->setStyleSheet("font-size: 18px; font-weight: bold; color: #e94560; margin-bottom: 10px;");
    layout->addWidget(header);

    QFormLayout *form = new QFormLayout;
    form->addRow("Teacher ID:", new QLabel(QString::number(t.id)));
    form->addRow("Gender:", new QLabel(t.gender.isEmpty() ? "Not Set" : t.gender));
    form->addRow("Date of Birth:", new QLabel(t.dateOfBirth.isEmpty() ? "Not Set" : t.dateOfBirth));
    form->addRow("Phone:", new QLabel(t.phone));
    form->addRow("Email:", new QLabel(t.email));
    
    SubjectManager sm;
    form->addRow("Specialization:", new QLabel(sm.getSubjectName(t.subject_id)));
    
    layout->addLayout(form);
    
    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    
    if (userRole == "admin") {
        QPushButton *resetPassBtn = new QPushButton("Reset Password");
        resetPassBtn->setCursor(Qt::PointingHandCursor);
        resetPassBtn->setStyleSheet(
            "QPushButton { background: #c0392b; color: white; border-radius: 4px; padding: 6px 12px; }"
            "QPushButton:hover { background: #a93226; }");
        btnRow->addWidget(resetPassBtn);
        
        connect(resetPassBtn, &QPushButton::clicked, this, [this, t]() {
            if (QMessageBox::question(this, "Reset Password", 
                "Are you sure you want to reset this teacher's password to the default ('pass" + QString::number(t.id) + "')?\n\nThis action cannot be undone.",
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                
                UserManager um;
                if (um.changePassword(QString::number(t.id), "pass" + QString::number(t.id))) {
                    QMessageBox::information(this, "Success", "Password reset successfully.");
                } else {
                    QMessageBox::warning(this, "Error", "Failed to reset password.");
                }
            }
        });
    }

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setObjectName("secondaryButton");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    layout->addLayout(btnRow);
}
