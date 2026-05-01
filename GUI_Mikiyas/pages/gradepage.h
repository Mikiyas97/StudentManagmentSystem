#ifndef GRADEPAGE_H
#define GRADEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include "../managers/grademanager.h"

class GradePage : public QWidget {
    Q_OBJECT
public:
    explicit GradePage(const QString &role = "admin", int studentId = -1,
                       QWidget *parent = nullptr);

private slots:
    void onAdd();
    void onFilter();
    void refreshTable(int filterId = -1);

private:
    GradeManager manager;
    QTableWidget *table;
    QSpinBox *sidSpin;
    QLineEdit *courseEdit;
    QDoubleSpinBox *scoreSpin;
    QSpinBox *filterSpin;
    QString userRole;
    int userStudentId;
};

#endif // GRADEPAGE_H
