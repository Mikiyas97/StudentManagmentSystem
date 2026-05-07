#include "usermanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

UserManager::UserManager() {}

QString UserManager::hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha256
    );
    return QString(hash.toHex());
}

bool UserManager::addUser(const User &u) {
    QSqlQuery q;
    q.prepare("REPLACE INTO users (username, password, role, relatedId) VALUES (?, ?, ?, ?)");
    q.addBindValue(u.username);
    q.addBindValue(hashPassword(u.password));
    q.addBindValue(u.role);
    q.addBindValue(u.relatedId);
    return q.exec();
}

bool UserManager::changePassword(const QString &username, const QString &newPassword) {
    QSqlQuery q;
    q.prepare("UPDATE users SET password = ? WHERE username = ?");
    q.addBindValue(hashPassword(newPassword));
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
    q.prepare("INSERT INTO teachers (id, fullName, gender, date_of_birth, phone, email, subject_id) VALUES (?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(t.id);
    q.addBindValue(t.fullName);
    q.addBindValue(t.gender);
    q.addBindValue(t.dateOfBirth);
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
    QSqlQuery q("SELECT id, fullName, gender, date_of_birth, phone, email, subject_id FROM teachers");
    while (q.next()) {
        Teacher t;
        t.id = q.value(0).toInt();
        t.fullName = q.value(1).toString();
        t.gender = q.value(2).toString();
        t.dateOfBirth = q.value(3).toString();
        t.phone = q.value(4).toString();
        t.email = q.value(5).toString();
        t.subject_id = q.value(6).toInt();
        list.push_back(t);
    }
    return list;
}

QVector<Teacher> UserManager::filterTeachers(const QString &searchText, const QString &sortBy) const {
    QVector<Teacher> list;
    QSqlQuery q;
    QString queryStr = "SELECT id, fullName, gender, date_of_birth, phone, email, subject_id FROM teachers";
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
            t.gender = q.value(2).toString();
            t.dateOfBirth = q.value(3).toString();
            t.phone = q.value(4).toString();
            t.email = q.value(5).toString();
            t.subject_id = q.value(6).toInt();
            list.push_back(t);
        }
    }
    return list;
}

Teacher UserManager::getTeacherById(int id) const {
    Teacher t;
    t.id = -1;
    QSqlQuery q;
    q.prepare("SELECT id, fullName, gender, date_of_birth, phone, email, subject_id FROM teachers WHERE id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        t.id = q.value(0).toInt();
        t.fullName = q.value(1).toString();
        t.gender = q.value(2).toString();
        t.dateOfBirth = q.value(3).toString();
        t.phone = q.value(4).toString();
        t.email = q.value(5).toString();
        t.subject_id = q.value(6).toInt();
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

QVector<GradeLevel> UserManager::getTeacherGrades(int teacherId) const {
    QVector<GradeLevel> list;
    QSqlQuery q;
    q.prepare("SELECT DISTINCT g.id, g.name FROM grade_levels g "
              "JOIN sections s ON g.id = s.grade_id "
              "JOIN teaching_assignments ta ON s.id = ta.section_id "
              "WHERE ta.teacher_id = ? ORDER BY g.name + 0 ASC");
    q.addBindValue(teacherId);
    if (q.exec()) {
        while (q.next()) {
            list.push_back({q.value("id").toInt(), q.value("name").toString()});
        }
    }
    return list;
}

QVector<Section> UserManager::getTeacherSections(int teacherId, int gradeId) const {
    QVector<Section> list;
    QSqlQuery sq;
    sq.prepare("SELECT DISTINCT s.id, s.name FROM sections s "
               "JOIN teaching_assignments ta ON s.id = ta.section_id "
               "WHERE ta.teacher_id = ? AND s.grade_id = ?");
    sq.addBindValue(teacherId);
    sq.addBindValue(gradeId);
    if (sq.exec()) {
        while (sq.next()) {
            Section s;
            s.id = sq.value("id").toInt();
            s.name = sq.value("name").toString();
            list.push_back(s);
        }
    }
    return list;
}

QVector<Subject> UserManager::getTeacherSubjects(int teacherId, int sectionId) const {
    QVector<Subject> list;
    QSqlQuery subq;
    subq.prepare("SELECT DISTINCT sub.id, sub.name FROM subjects sub "
                 "JOIN teaching_assignments ta ON sub.id = ta.subject_id "
                 "WHERE ta.teacher_id = ? AND ta.section_id = ?");
    subq.addBindValue(teacherId);
    subq.addBindValue(sectionId);
    if (subq.exec()) {
        while (subq.next()) {
            Subject s;
            s.id = subq.value("id").toInt();
            s.name = subq.value("name").toString();
            list.push_back(s);
        }
    }
    return list;
}

QVector<Teacher> UserManager::getTeachersBySubject(int subjectId) const {
    QVector<Teacher> list;
    QSqlQuery q;
    q.prepare("SELECT id, fullName FROM teachers WHERE subject_id = ?");
    q.addBindValue(subjectId);
    if (q.exec()) {
        while (q.next()) {
            Teacher t;
            t.id = q.value("id").toInt();
            t.fullName = q.value("fullName").toString();
            list.push_back(t);
        }
    }
    return list;
}
