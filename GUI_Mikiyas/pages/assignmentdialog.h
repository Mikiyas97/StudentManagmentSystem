#ifndef ASSIGNMENTDIALOG_H
#define ASSIGNMENTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include "../managers/subjectmanager.h"

class AssignmentDialog : public QDialog {
    Q_OBJECT
public:
    explicit AssignmentDialog(QWidget *parent = nullptr);

private slots:
    void onGradeChanged();
    void onSectionChanged();
    void onSubjectChanged();
    void onAssign();

private:
    QComboBox *gradeCombo;
    QComboBox *sectionCombo;
    QComboBox *subjectCombo;
    QComboBox *teacherCombo;
    
    SubjectManager subManager;
};

#endif // ASSIGNMENTDIALOG_H
