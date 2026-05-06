#include "profilepage.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

ProfilePage::ProfilePage(const QString &role, int id, QWidget *parent)
    : QWidget(parent), userRole(role), userId(id)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("My Profile");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    if (userRole == "admin") setupAdminProfile();
    else if (userRole == "teacher") setupTeacherProfile(userId);
    else setupStudentProfile(userId);

    layout->addStretch();
}

void ProfilePage::setupAdminProfile() {
    QGroupBox *box = new QGroupBox("Admin Information");
    QFormLayout *form = new QFormLayout(box);
    form->addRow("Username:", new QLabel("admin"));
    form->addRow("Role:", new QLabel("System Administrator"));
    layout()->addWidget(box);
}

void ProfilePage::setupTeacherProfile(int id) {
    UserManager um;
    Teacher t = um.getTeacherById(id);

    QGroupBox *box = new QGroupBox("Teacher Details");
    QFormLayout *form = new QFormLayout(box);
    form->addRow("Teacher ID:", new QLabel(QString::number(t.id)));
    form->addRow("Full Name:", new QLabel(t.fullName));
    form->addRow("Phone:", new QLabel(t.phone));
    form->addRow("Email:", new QLabel(t.email));
    layout()->addWidget(box);
}

void ProfilePage::setupStudentProfile(int id) {
    StudentManager sm;
    Student s = sm.getStudentById(id);

    QGroupBox *box = new QGroupBox("Student Details");
    QFormLayout *form = new QFormLayout(box);
    form->addRow("Student ID:", new QLabel(QString::number(s.id)));
    form->addRow("Full Name:", new QLabel(s.fullName));
    form->addRow("Grade:", new QLabel(s.gradeName));
    form->addRow("Section:", new QLabel(s.sectionName));
    form->addRow("Stream:", new QLabel(s.streamName.isEmpty() ? "General" : s.streamName));
    form->addRow("Phone:", new QLabel(s.phone));
    form->addRow("Email:", new QLabel(s.email));
    form->addRow("Status:", new QLabel(s.status));
    layout()->addWidget(box);
}
