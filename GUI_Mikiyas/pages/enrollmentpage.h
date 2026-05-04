#ifndef ENROLLMENTPAGE_H
#define ENROLLMENTPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include "../managers/markmanager.h"

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
    QTableWidget *table;
    QComboBox *sectionFilter;
    QComboBox *subjectFilter;
    QLabel *averageLabel;
    QString userRole;
    int userStudentId;
};

#endif // ENROLLMENTPAGE_H
