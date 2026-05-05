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
    q.prepare("INSERT INTO teachers (id, fullName, phone, email, subject_id) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(t.id);
    q.addBindValue(t.fullName);
    q.addBindValue(t.phone);
    q.addBindValue(t.email);
    q.addBindValue(t.subject_id);
    if (q.exec()) {
        User u;
        u.username = QString::number(t.id);
        u.password = password;
        u.role = "teacher";
        u.relatedId = t.id;
        return addUser(u);
    }
    return false;
}

QVector<Teacher> UserManager::getTeachers() const {
    QVector<Teacher> list;
    QSqlQuery q("SELECT id, fullName, phone, email, subject_id FROM teachers");
    while (q.next()) {
        Teacher t;
        t.id = q.value(0).toInt();
        t.fullName = q.value(1).toString();
        t.phone = q.value(2).toString();
        t.email = q.value(3).toString();
        t.subject_id = q.value(4).toInt();
        list.push_back(t);
    }
    return list;
}

QVector<Teacher> UserManager::filterTeachers(const QString &searchText, const QString &sortBy) const {
    QVector<Teacher> list;
    QSqlQuery q;
    QString queryStr = "SELECT id, fullName, phone, email, subject_id FROM teachers";
    if (!searchText.isEmpty()) {
        queryStr += " WHERE fullName LIKE ? OR phone LIKE ? OR email LIKE ?";
    }
    
    if (!sortBy.isEmpty()) {
        queryStr += " ORDER BY " + sortBy;
    }
    
    q.prepare(queryStr);
    if (!searchText.isEmpty()) {
        QString likeStr = "%" + searchText + "%";
        q.addBindValue(likeStr);
        q.addBindValue(likeStr);
        q.addBindValue(likeStr);
    }
    
    if (q.exec()) {
        while (q.next()) {
            Teacher t;
            t.id = q.value(0).toInt();
            t.fullName = q.value(1).toString();
            t.phone = q.value(2).toString();
            t.email = q.value(3).toString();
            t.subject_id = q.value(4).toInt();
            list.push_back(t);
        }
    }
    return list;
}

Teacher UserManager::getTeacherById(int id) const {
    Teacher t;
    t.id = -1;
    QSqlQuery q;
    q.prepare("SELECT id, fullName, phone, email, subject_id FROM teachers WHERE id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        t.id = q.value(0).toInt();
        t.fullName = q.value(1).toString();
        t.phone = q.value(2).toString();
        t.email = q.value(3).toString();
        t.subject_id = q.value(4).toInt();
    }
    return t;
}

bool UserManager::deleteTeacher(int id) {
    QSqlQuery q;
    q.prepare("DELETE FROM teachers WHERE id = ?");
    q.addBindValue(id);
    if (q.exec()) {
        QSqlQuery q2;
        q2.prepare("DELETE FROM users WHERE relatedId = ? AND role = 'teacher'");
        q2.addBindValue(id);
        q2.exec();
        
        // Also remove assignments
        QSqlQuery q3;
        q3.prepare("DELETE FROM teaching_assignments WHERE teacher_id = ?");
        q3.addBindValue(id);
        q3.exec();
        
        return true;
    }
    return false;
}
