#include "studentmanager.h"
#include "usermanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

StudentManager::StudentManager() {
    // Database connection is globally handled in main.cpp
}

// --------------- File I/O is gone ---------------
void StudentManager::loadFromFile() {}
void StudentManager::saveToFile() {}

// --------------- CRUD ---------------

int StudentManager::generateNextId() const {
    QSqlQuery query("SELECT MAX(id) FROM students");
    if (query.next()) {
        return query.value(0).toInt() + 1;
    }
    return 1;
}

bool StudentManager::addStudent(const Student &s) {
    QSqlQuery query;
    query.prepare("INSERT INTO students (id, fullName, age, gender, className, phone, email, address, guardianName, guardianContact, status) "
                  "VALUES (:id, :name, :age, :gender, :class, :phone, :email, :addr, :gname, :gphone, :status)");
    query.bindValue(":id", s.id);
    query.bindValue(":name", s.fullName);
    query.bindValue(":age", s.age);
    query.bindValue(":gender", s.gender);
    query.bindValue(":class", s.className);
    query.bindValue(":phone", s.phone);
    query.bindValue(":email", s.email);
    query.bindValue(":addr", s.address);
    query.bindValue(":gname", s.guardianName);
    query.bindValue(":gphone", s.guardianContact);
    query.bindValue(":status", s.status);
    if (query.exec()) {
        UserManager um;
        User u;
        u.username = QString::number(s.id);
        u.password = "pass" + QString::number(s.id);
        u.role = "student";
        u.relatedId = s.id;
        um.addUser(u);
        return true;
    }
    return false;
}

bool StudentManager::updateStudent(const Student &s) {
    QSqlQuery query;
    query.prepare("UPDATE students SET fullName=:name, age=:age, gender=:gender, className=:class, phone=:phone, "
                  "email=:email, address=:addr, guardianName=:gname, guardianContact=:gphone, status=:status WHERE id=:id");
    query.bindValue(":id", s.id);
    query.bindValue(":name", s.fullName);
    query.bindValue(":age", s.age);
    query.bindValue(":gender", s.gender);
    query.bindValue(":class", s.className);
    query.bindValue(":phone", s.phone);
    query.bindValue(":email", s.email);
    query.bindValue(":addr", s.address);
    query.bindValue(":gname", s.guardianName);
    query.bindValue(":gphone", s.guardianContact);
    query.bindValue(":status", s.status);
    return query.exec();
}

bool StudentManager::softDeleteStudent(int id) {
    QSqlQuery query;
    query.prepare("UPDATE students SET status='Inactive' WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

bool StudentManager::hardDeleteStudent(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

void StudentManager::bulkSoftDelete(const QVector<int> &ids) {
    QSqlQuery query;
    query.prepare("UPDATE students SET status='Inactive' WHERE id=:id");
    for (int id : ids) {
        query.bindValue(":id", id);
        query.exec();
    }
}

void StudentManager::bulkHardDelete(const QVector<int> &ids) {
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE id=:id");
    for (int id : ids) {
        query.bindValue(":id", id);
        query.exec();
    }
}

void StudentManager::bulkAssignClass(const QVector<int> &ids, const QString &cls) {
    QSqlQuery query;
    query.prepare("UPDATE students SET className=:class WHERE id=:id");
    for (int id : ids) {
        query.bindValue(":id", id);
        query.bindValue(":class", cls);
        query.exec();
    }
}

// --------------- Query ---------------

static Student parseStudent(QSqlQuery &q) {
    Student s;
    s.id = q.value("id").toInt();
    s.fullName = q.value("fullName").toString();
    s.age = q.value("age").toInt();
    s.gender = q.value("gender").toString();
    s.className = q.value("className").toString();
    s.phone = q.value("phone").toString();
    s.email = q.value("email").toString();
    s.address = q.value("address").toString();
    s.guardianName = q.value("guardianName").toString();
    s.guardianContact = q.value("guardianContact").toString();
    s.status = q.value("status").toString();
    return s;
}

QVector<Student> StudentManager::getStudents() const {
    QVector<Student> list;
    QSqlQuery query("SELECT * FROM students");
    while (query.next()) list.push_back(parseStudent(query));
    return list;
}

Student StudentManager::getStudentById(int id) const {
    QSqlQuery query;
    query.prepare("SELECT * FROM students WHERE id=:id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return parseStudent(query);
    }
    Student empty; empty.id = -1; return empty;
}

QVector<Student> StudentManager::filter(const QString &nameOrId,
                                         const QString &classFilter,
                                         const QString &statusFilter) const {
    QString sql = "SELECT * FROM students WHERE 1=1";
    
    if (!nameOrId.isEmpty()) {
        sql += " AND (id LIKE '%" + nameOrId + "%' OR fullName LIKE '%" + nameOrId + "%')";
    }
    if (!classFilter.isEmpty() && classFilter != "All") {
        sql += " AND className = '" + classFilter + "'";
    }
    if (!statusFilter.isEmpty() && statusFilter != "All") {
        sql += " AND status = '" + statusFilter + "'";
    }
    
    if (currentSortField == ByName) {
        sql += " ORDER BY fullName " + QString(currentSortAscending ? "ASC" : "DESC");
    } else if (currentSortField == ByClass) {
        sql += " ORDER BY className " + QString(currentSortAscending ? "ASC" : "DESC");
    } else {
        sql += " ORDER BY id " + QString(currentSortAscending ? "ASC" : "DESC");
    }

    QVector<Student> list;
    QSqlQuery query(sql);
    while (query.next()) list.push_back(parseStudent(query));
    return list;
}

QVector<Student> StudentManager::getStudentsByCourse(const QString &courseCode) const {
    QVector<Student> list;
    QSqlQuery query;
    query.prepare("SELECT s.* FROM students s "
                  "JOIN enrollments e ON s.id = e.studentId "
                  "WHERE e.courseCode = ?");
    query.addBindValue(courseCode);
    if (query.exec()) {
        while (query.next()) {
            list.push_back(parseStudent(query));
        }
    }
    return list;
}

// --------------- Sorting ---------------

void StudentManager::sortBy(SortField field, bool ascending) {
    currentSortField = field;
    currentSortAscending = ascending;
}
