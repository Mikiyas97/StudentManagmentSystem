#ifndef COURSEPAGE_H
#define COURSEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include "../managers/coursemanager.h"

class CoursePage : public QWidget {
    Q_OBJECT
public:
    explicit CoursePage(QWidget *parent = nullptr);

private slots:
    void onAdd();
    void refreshTable();

private:
    CourseManager manager;
    QTableWidget *table;
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QSpinBox *creditsSpin;
};

#endif // COURSEPAGE_H
