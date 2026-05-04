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
    query.prepare("INSERT INTO students (id, fullName, grade_id, section_id, stream_id, phone, email, status) "
                  "VALUES (:id, :name, :grade, :section, :stream, :phone, :email, :status)");
    query.bindValue(":id", s.id);
    query.bindValue(":name", s.fullName);
    query.bindValue(":grade", s.grade_id);
    query.bindValue(":section", s.section_id);
    query.bindValue(":stream", s.stream_id > 0 ? QVariant(s.stream_id) : QVariant(QVariant::Int));
    query.bindValue(":phone", s.phone);
    query.bindValue(":email", s.email);
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
    query.prepare("UPDATE students SET fullName=:name, grade_id=:grade, section_id=:section, stream_id=:stream, phone=:phone, "
                  "email=:email, status=:status WHERE id=:id");
    query.bindValue(":id", s.id);
    query.bindValue(":name", s.fullName);
    query.bindValue(":grade", s.grade_id);
    query.bindValue(":section", s.section_id);
    query.bindValue(":stream", s.stream_id > 0 ? QVariant(s.stream_id) : QVariant(QVariant::Int));
    query.bindValue(":phone", s.phone);
    query.bindValue(":email", s.email);
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

void StudentManager::bulkAssignClass(const QVector<int> &ids, const QString &sectionIdStr) {
    QSqlQuery query;
    query.prepare("UPDATE students SET section_id=:sec WHERE id=:id");
    int secId = sectionIdStr.toInt();
    for (int id : ids) {
        query.bindValue(":id", id);
        query.bindValue(":sec", secId);
        query.exec();
    }
}

// --------------- Query ---------------

static Student parseStudent(QSqlQuery &q) {
    Student s;
    s.id = q.value("id").toInt();
    s.fullName = q.value("fullName").toString();
    s.grade_id = q.value("grade_id").toInt();
    s.gradeName = q.value("grade_name").toString();
    s.section_id = q.value("section_id").toInt();
    s.sectionName = q.value("section_name").toString();
    s.stream_id = q.value("stream_id").toInt();
    s.streamName = q.value("stream_name").toString();
    s.phone = q.value("phone").toString();
    s.email = q.value("email").toString();
    s.status = q.value("status").toString();
    return s;
}

QVector<Student> StudentManager::getStudents() const {
    QVector<Student> list;
    QSqlQuery query("SELECT s.*, g.name as grade_name, sec.name as section_name, st.name as stream_name "
                   "FROM students s "
                   "LEFT JOIN grade_levels g ON s.grade_id = g.id "
                   "LEFT JOIN sections sec ON s.section_id = sec.id "
                   "LEFT JOIN streams st ON s.stream_id = st.id");
    while (query.next()) list.push_back(parseStudent(query));
    return list;
}

Student StudentManager::getStudentById(int id) const {
    QSqlQuery query;
    query.prepare("SELECT s.*, g.name as grade_name, sec.name as section_name, st.name as stream_name "
                  "FROM students s "
                  "LEFT JOIN grade_levels g ON s.grade_id = g.id "
                  "LEFT JOIN sections sec ON s.section_id = sec.id "
                  "LEFT JOIN streams st ON s.stream_id = st.id "
                  "WHERE s.id=:id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return parseStudent(query);
    }
    Student empty; empty.id = -1; return empty;
}

QVector<Student> StudentManager::filter(const QString &nameOrId,
                                         const QString &gradeFilter,
                                         const QString &statusFilter) const {
    QString sql = "SELECT s.*, g.name as grade_name, sec.name as section_name, st.name as stream_name "
                  "FROM students s "
                  "LEFT JOIN grade_levels g ON s.grade_id = g.id "
                  "LEFT JOIN sections sec ON s.section_id = sec.id "
                  "LEFT JOIN streams st ON s.stream_id = st.id "
                  "WHERE 1=1";
    
    if (!nameOrId.isEmpty()) {
        sql += " AND (s.id LIKE '%" + nameOrId + "%' OR s.fullName LIKE '%" + nameOrId + "%')";
    }
    if (!gradeFilter.isEmpty() && gradeFilter != "All") {
        sql += " AND g.name = '" + gradeFilter + "'";
    }
    if (!statusFilter.isEmpty() && statusFilter != "All") {
        sql += " AND s.status = '" + statusFilter + "'";
    }
    
    if (currentSortField == ByName) {
        sql += " ORDER BY s.fullName " + QString(currentSortAscending ? "ASC" : "DESC");
    } else if (currentSortField == ByClass) {
        sql += " ORDER BY sec.name " + QString(currentSortAscending ? "ASC" : "DESC");
    } else {
        sql += " ORDER BY s.id " + QString(currentSortAscending ? "ASC" : "DESC");
    }

    QVector<Student> list;
    QSqlQuery query(sql);
    while (query.next()) list.push_back(parseStudent(query));
    return list;
}

QVector<Student> StudentManager::getStudentsByCourse(const QString &courseCode) const {
    // This needs to be refactored to getStudentsBySection or similar
    QVector<Student> list;
    return list;
}

// --------------- Sorting ---------------

void StudentManager::sortBy(SortField field, bool ascending) {
    currentSortField = field;
    currentSortAscending = ascending;
}
