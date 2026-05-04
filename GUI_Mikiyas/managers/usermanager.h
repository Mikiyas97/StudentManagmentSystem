#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QVector>

struct Teacher {
    int id;
    QString fullName;
    QString phone;
    QString email;
};

struct User {
    QString username;
    QString password;
    QString role;
    int relatedId;
};

class UserManager {
public:
    UserManager();
    
    // Users
    bool addUser(const User &u);
    bool changePassword(const QString &username, const QString &newPassword);
    User getUserByUsername(const QString &username) const;
    
    // Teachers
    int generateNextTeacherId() const;
    bool addTeacher(const Teacher &t, const QString &password);
    QVector<Teacher> getTeachers() const;
    QVector<Teacher> filterTeachers(const QString &searchText, const QString &sortBy = "") const;
    Teacher getTeacherById(int id) const;
};

#endif // USERMANAGER_H
