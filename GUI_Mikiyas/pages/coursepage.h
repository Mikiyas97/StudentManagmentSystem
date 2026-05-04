#ifndef COURSEPAGE_H
#define COURSEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include "../managers/subjectmanager.h"

class CoursePage : public QWidget {
    Q_OBJECT
public:
    explicit CoursePage(QWidget *parent = nullptr);

private slots:
    void onAdd();
    void onDelete();
    void refreshTable();

private:
    SubjectManager manager;
    QTableWidget *table;
    QLineEdit *nameEdit;
    QComboBox *gradeCombo;
    QComboBox *streamCombo;
};

#endif // COURSEPAGE_H
