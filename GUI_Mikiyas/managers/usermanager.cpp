#include "usermanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

UserManager::UserManager() {}

bool UserManager::addUser(const User &u) {
    QSqlQuery q;
    q.prepare("INSERT OR REPLACE INTO users (username, password, role, relatedId) VALUES (?, ?, ?, ?)");
    q.addBindValue(u.username);
    q.addBindValue(u.password);
    q.addBindValue(u.role);
    q.addBindValue(u.relatedId);
    return q.exec();
}

bool UserManager::changePassword(const QString &username, const QString &newPassword) {
    QSqlQuery q;
    q.prepare("UPDATE users SET password = ? WHERE username = ?");
    q.addBindValue(newPassword);
    q.addBindValue(username);
    return q.exec();
}

User UserManager::getUserByUsername(const QString &username) const {
    QSqlQuery q;
    q.prepare("SELECT username, password, role, relatedId FROM users WHERE username = ?");
    q.addBindValue(username);
    User u;
    if (q.exec() && q.next()) {
        u.username = q.value(0).toString();
        u.password = q.value(1).toString();
        u.role = q.value(2).toString();
        u.relatedId = q.value(3).toInt();
    }
    return u;
}

int UserManager::generateNextTeacherId() const {
    QSqlQuery q("SELECT MAX(id) FROM teachers");
    if (q.next()) {
        int maxId = q.value(0).toInt();
        if (maxId > 0) return maxId + 1;
    }
    return 2001; // Start from 2001 to avoid conflict with students (1-1000)
}

bool UserManager::addTeacher(const Teacher &t, const QString &password) {
    QSqlQuery q;
    q.prepare("INSERT INTO teachers (id, fullName, courseCode, phone, email) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(t.id);
    q.addBindValue(t.fullName);
    q.addBindValue(t.courseCode);
    q.addBindValue(t.phone);
    q.addBindValue(t.email);
    if (q.exec()) {
        User u;
        u.username = QString::number(t.id); // username is teacher ID
        u.password = password;
        u.role = "teacher";
        u.relatedId = t.id;
        if (!addUser(u)) {
            qDebug() << "Failed to add user for teacher:" << q.lastError().text();
            return false;
        }
        return true;
    }
    qDebug() << "Failed to add teacher to teachers table:" << q.lastError().text();
    return false;
}

QVector<Teacher> UserManager::getTeachers() const {
    QVector<Teacher> list;
    QSqlQuery q("SELECT id, fullName, courseCode, phone, email FROM teachers");
    while (q.next()) {
        Teacher t;
        t.id = q.value(0).toInt();
        t.fullName = q.value(1).toString();
        t.courseCode = q.value(2).toString();
        t.phone = q.value(3).toString();
        t.email = q.value(4).toString();
        list.push_back(t);
    }
    return list;
}

Teacher UserManager::getTeacherById(int id) const {
    Teacher t;
    t.id = -1;
    QSqlQuery q;
    q.prepare("SELECT id, fullName, courseCode, phone, email FROM teachers WHERE id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        t.id = q.value(0).toInt();
        t.fullName = q.value(1).toString();
        t.courseCode = q.value(2).toString();
        t.phone = q.value(3).toString();
        t.email = q.value(4).toString();
    }
    return t;
}
