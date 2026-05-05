#include "teacherdetaildialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

TeacherDetailDialog::TeacherDetailDialog(const Teacher &t, QWidget *parent) : QDialog(parent) {
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
    
    layout->addLayout(form);

    QPushButton *closeBtn = new QPushButton("Close");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn);
}
