#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QWidget>
#include <QLabel>
#include "managers/usermanager.h"
#include "managers/studentmanager.h"

class ProfilePage : public QWidget {
    Q_OBJECT
public:
    explicit ProfilePage(const QString &role, int id, QWidget *parent = nullptr);

private:
    void setupAdminProfile();
    void setupTeacherProfile(int id);
    void setupStudentProfile(int id);

    QString userRole;
    int userId;
};

#endif // PROFILEPAGE_H
