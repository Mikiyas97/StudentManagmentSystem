#ifndef TEACHERPAGE_H
#define TEACHERPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include "managers/usermanager.h"

class TeacherPage : public QWidget {
    Q_OBJECT
public:
    explicit TeacherPage(QWidget *parent = nullptr);

private slots:
    void onAddTeacher();
    void onRowDoubleClicked(int row, int column);
    void onSortChanged(int index);
    void refreshTable();

private:
    QTableWidget *table;
    QLineEdit *searchEdit;
    QComboBox *sortCombo;
    UserManager manager;
};

#endif // TEACHERPAGE_H
