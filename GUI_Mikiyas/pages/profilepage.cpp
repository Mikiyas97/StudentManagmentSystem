#include "profilepage.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QFrame>

ProfilePage::ProfilePage(const QString &role, int id, QWidget *parent)
    : QWidget(parent), userRole(role), userId(id)
{
    // 1. Set Dark Theme Background
    this->setStyleSheet("background-color: #1a1a2e; color: #eaeaea; font-family: 'Segoe UI';");

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(15);

    // 2. Logic to load data and build UI
    if (userRole.toLower() == "admin") {
        createHeader("System Administrator", "ID: 0");
        setupAdminProfile();
    } else if (userRole.toLower() == "teacher") {
        UserManager um;
        Teacher t = um.getTeacherById(id);
        createHeader(t.fullName, "ID: " + QString::number(t.id));
        setupTeacherProfile(id);
    } else {
        StudentManager sm;
        Student s = sm.getStudentById(id);
        createHeader(s.fullName, "ID: " + QString::number(s.id));
        setupStudentProfile(id);
    }

    mainLayout->addStretch();
}

void ProfilePage::createHeader(const QString &name, const QString &idText) {
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(20);
    headerLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    // Avatar Placeholder (Icon)
    QLabel *avatar = new QLabel();
    avatar->setFixedSize(80, 80);
    avatar->setStyleSheet("background-color: #3d345c; border-radius: 40px;"); // Circular purple bg
    
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    textLayout->setAlignment(Qt::AlignVCenter);

    QLabel *nameLabel = new QLabel(name); // Removed forced lowercase
    nameLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #e94560;");
    
    QLabel *idLabel = new QLabel(idText);
    idLabel->setStyleSheet("color: #a0a0b0; font-size: 14px;");

    textLayout->addWidget(nameLabel);
    textLayout->addWidget(idLabel);

    headerLayout->addWidget(avatar);
    headerLayout->addLayout(textLayout);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    // Separator Line
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #1f4068;");
    mainLayout->addWidget(line);
}

QWidget* ProfilePage::createInfoGroup(const QString &title, QMap<QString, QString> details) {
    QGroupBox *box = new QGroupBox(title);
    // Custom Styling for the GroupBox to match the blue containers
    box->setStyleSheet(
        "QGroupBox { border: 1px solid #1f4068; border-radius: 8px; "
        "background-color: #16213e; margin-top: 15px; padding-top: 20px; font-weight: bold; color: #e94560; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; }"
    );

    QFormLayout *form = new QFormLayout(box);
    form->setVerticalSpacing(12);
    form->setLabelAlignment(Qt::AlignLeft);

    QMapIterator<QString, QString> i(details);
    while (i.hasNext()) {
        i.next();
        QLabel *valLabel = new QLabel(i.value());
        valLabel->setStyleSheet("color: #eaeaea; font-weight: normal;");
        
        QLabel *keyLabel = new QLabel(i.key() + ":");
        keyLabel->setStyleSheet("color: #a0a0b0; font-weight: bold; min-width: 100px;");
        
        form->addRow(keyLabel, valLabel);
    }

    return box;
}

void ProfilePage::setupStudentProfile(int id) {
    StudentManager sm;
    Student s = sm.getStudentById(id);

    // Personal Info Group
    QMap<QString, QString> personal;
    personal["Gender"] = "Male"; // Add this field to your Student struct if available
    personal["Date of Birth"] = "23/10/1997"; 
    personal["Phone"] = s.phone;
    personal["Email"] = s.email;
    mainLayout->addWidget(createInfoGroup("Personal Information", personal));

    // Academic Placement Group
    QMap<QString, QString> academic;
    academic["Grade"] = s.gradeName;
    academic["Section"] = s.sectionName;
    academic["Stream"] = s.streamName.isEmpty() ? "General" : s.streamName;
    mainLayout->addWidget(createInfoGroup("Academic Placement", academic));
}

void ProfilePage::setupTeacherProfile(int id) {
    UserManager um;
    Teacher t = um.getTeacherById(id);
    
    SubjectManager sm;
    QString subjectName = sm.getSubjectName(t.subject_id);
    
    QMap<QString, QString> personal;
    personal["Phone"] = t.phone;
    personal["Email"] = t.email;
    personal["Specialization"] = subjectName;
    mainLayout->addWidget(createInfoGroup("Teacher Details", personal));
}

void ProfilePage::setupAdminProfile() {
    QMap<QString, QString> details;
    details["Username"] = "admin";
    details["Role"] = "System Administrator";
    mainLayout->addWidget(createInfoGroup("Admin Information", details));
}

void ProfilePage::refreshTable() {
    // Empty stub to suppress QMetaObject::invokeMethod warnings.
    // The profile page is mostly static and doesn't need to refresh its tables on every tab switch.
}