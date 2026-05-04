#ifndef TEACHERDETAILDIALOG_H
#define TEACHERDETAILDIALOG_H

#include <QDialog>
#include "managers/usermanager.h"

class TeacherDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit TeacherDetailDialog(const Teacher &t, QWidget *parent = nullptr);
};

#endif // TEACHERDETAILDIALOG_H
