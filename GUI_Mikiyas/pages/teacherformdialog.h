#ifndef TEACHERFORMDIALOG_H
#define TEACHERFORMDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "managers/usermanager.h"

class TeacherFormDialog : public QDialog {
    Q_OBJECT
public:
    explicit TeacherFormDialog(QWidget *parent = nullptr);

    Teacher getTeacher() const;
    int getSubjectId() const;
    int getSectionId() const;

private slots:
    void onSave();

private:
    QLineEdit *nameEdit;
    QLineEdit *phoneEdit;
    QLineEdit *emailEdit;
    QComboBox *subjectCombo;
    QComboBox *sectionCombo;
    
    Teacher resultTeacher;
    int resultSubjectId;
    int resultSectionId;
};

#endif // TEACHERFORMDIALOG_H
