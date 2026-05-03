#ifndef ENROLLMENTPAGE_H
#define ENROLLMENTPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QLineEdit>
#include "managers/enrollmentmanager.h"
#include "managers/studentmanager.h"

class EnrollmentPage : public QWidget {
    Q_OBJECT
public:
    explicit EnrollmentPage(QWidget *parent = nullptr);

private slots:
    void onEnroll();
    void onUnenroll();
    void onSearch();

private:
    void refreshTable(int studentId);

    QTableWidget *table;
    QSpinBox *studentIdSpin;
    QLineEdit *courseCodeEdit;
    QSpinBox *searchSpin;
    
    EnrollmentManager manager;
    StudentManager studentManager;
};

#endif // ENROLLMENTPAGE_H
