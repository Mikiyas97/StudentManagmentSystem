#ifndef STUDENTDETAILDIALOG_H
#define STUDENTDETAILDIALOG_H

#include <QDialog>
#include "../managers/studentmanager.h"

#include "../managers/markmanager.h"

class StudentDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit StudentDetailDialog(const Student &s, QWidget *parent = nullptr);

signals:
    void editRequested(int studentId);

private:
    Student student;
    MarkManager markManager;
};

#endif // STUDENTDETAILDIALOG_H
