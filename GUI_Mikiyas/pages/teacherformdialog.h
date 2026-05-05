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

private slots:
    void onSave();

private:
    QLineEdit *nameEdit;
    QComboBox *genderCombo;
    QLineEdit *dobEdit;
    QLineEdit *phoneEdit;
    QLineEdit *emailEdit;
    QComboBox *subjectCombo;
    
    Teacher resultTeacher;
};

#endif // TEACHERFORMDIALOG_H
