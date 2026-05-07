#ifndef COURSEPAGE_H
#define COURSEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include "../managers/subjectmanager.h"
#include "../managers/sectionmanager.h"

class CoursePage : public QWidget {
    Q_OBJECT
public:
    explicit CoursePage(QWidget *parent = nullptr);

public slots:
    void refreshTable();

private slots:
    void onAdd();
    void onDelete();

private:
    SubjectManager manager;
    SectionManager sectionManager;
    QTableWidget *table;
    QLineEdit *nameEdit;
    QComboBox *gradeCombo;
    QComboBox *streamCombo;
};

#endif // COURSEPAGE_H
