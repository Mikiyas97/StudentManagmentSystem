#ifndef STUDENTFORMDIALOG_H
#define STUDENTFORMDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include "../managers/studentmanager.h"

class StudentFormDialog : public QDialog {
    Q_OBJECT
public:
    // Pass existing student for edit mode; default-constructed = add mode
    explicit StudentFormDialog(const Student &s = Student(), bool editMode = false,
                               QWidget *parent = nullptr);
    Student getStudent() const;

private slots:
    void onSave();

private:
    bool editing;
    QSpinBox    *idSpin;
    QLineEdit   *nameEdit;
    QSpinBox    *ageSpin;
    QComboBox   *genderCombo;
    QLineEdit   *classEdit;
    QLineEdit   *phoneEdit;
    QLineEdit   *emailEdit;
    QTextEdit   *addressEdit;
    QLineEdit   *guardianEdit;
    QLineEdit   *guardianPhoneEdit;
    QComboBox   *statusCombo;
    Student      result;
};

#endif // STUDENTFORMDIALOG_H
