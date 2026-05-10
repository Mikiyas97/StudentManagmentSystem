#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>

struct UserSession {
    QString role;
    int studentId;
};

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getRole() const;
    int getStudentId() const;

private slots:
    void onLogin();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLabel *errorLabel;
    QString role;
    int studentId;
};

#endif // LOGINDIALOG_H
