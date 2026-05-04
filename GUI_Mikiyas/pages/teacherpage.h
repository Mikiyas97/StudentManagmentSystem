#ifndef TEACHERPAGE_H
#define TEACHERPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include "managers/usermanager.h"

class TeacherPage : public QWidget {
    Q_OBJECT
public:
    explicit TeacherPage(QWidget *parent = nullptr);

private slots:
    void onAddTeacher();
    void refreshTable();

private:
    QTableWidget *table;
    QLineEdit *nameEdit;
    QComboBox *subjectCombo;
    QComboBox *sectionCombo;
    QLineEdit *phoneEdit;
    QLineEdit *emailEdit;
    UserManager manager;
};

#endif // TEACHERPAGE_H
