#ifndef ENROLLMENTPAGE_H
#define ENROLLMENTPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include "../managers/markmanager.h"
#include "../managers/sectionmanager.h"
#include "../managers/subjectmanager.h"
#include "../managers/usermanager.h"

class EnrollmentPage : public QWidget {
    Q_OBJECT
public:
    explicit EnrollmentPage(const QString &role = "admin", int studentId = -1, QWidget *parent = nullptr);

private slots:
    void onSaveAll();
    void onFilter();
    void refreshTable();

private:
    MarkManager manager;
    SectionManager sectionManager;
    SubjectManager subjectManager;
    UserManager userManager;
    
    QTableWidget *table;
    QComboBox *gradeCombo;
    QComboBox *streamCombo;
    QComboBox *sectionFilter;
    QComboBox *subjectFilter;
    QComboBox *semesterFilter;
    QLabel *averageLabel;
    QString userRole;
    int userStudentId;
};

#endif // ENROLLMENTPAGE_H
