#ifndef STUDENTPAGE_H
#define STUDENTPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include "../managers/studentmanager.h"

class StudentPage : public QWidget {
    Q_OBJECT
public:
    explicit StudentPage(const QString &role = "admin", int id = -1, QWidget *parent = nullptr);

private slots:
    void onAddStudent();
    void onEditStudent();
    void onViewStudent();
    void onDeleteStudent();
    void onBulkDelete();
    void onBulkAssignClass();
    void onSearch();
    void onSortChanged(int index);
    void onRowDoubleClicked(int row, int column);
    void refreshTable();
    void updateBulkBar();

private:
    StudentManager manager;
    QTableWidget *table;
    QLineEdit *searchEdit;
    QComboBox *classFilterCombo;
    QComboBox *statusFilterCombo;
    QComboBox *sortCombo;
    QWidget *bulkBar;
    QLabel *selectionLabel;
    QString userRole;
    QString teacherCourse;

    void editStudentById(int id);
    QVector<int> getCheckedIds() const;
    QStringList getUniqueClasses() const;
};

#endif // STUDENTPAGE_H
