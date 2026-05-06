#ifndef TEACHERPAGE_H
#define TEACHERPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include "managers/usermanager.h"

class TeacherPage : public QWidget {
    Q_OBJECT
public:
    explicit TeacherPage(QWidget *parent = nullptr);

public slots:
    void refreshTable();

private slots:
    void onAddTeacher();
    void onRowDoubleClicked(int row, int column);
    void onSortChanged(int index);
    void updateBulkBar();
    void onSearch();
    void onViewTeacher(int id);
    void onEditTeacher(int id);
    void onDeleteTeacher(int id);
    void onBulkDelete();

private:
    QTableWidget *table;
    QLineEdit *searchEdit;
    QComboBox *sortCombo;
    QComboBox *subjectFilterCombo;
    QWidget *bulkBar;
    QLabel *selectionLabel;
    UserManager manager;
    SubjectManager subjectManager;
    
    QVector<int> getCheckedIds() const;
};

#endif // TEACHERPAGE_H
