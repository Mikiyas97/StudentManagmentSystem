#include "subjectmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

SubjectManager::SubjectManager() {}

QVector<Subject> SubjectManager::getAllSubjects() const {
    QVector<Subject> list;
    QSqlQuery query("SELECT * FROM subjects ORDER BY name ASC");
    while (query.next()) {
        Subject s;
        s.id = query.value("id").toInt();
        s.name = query.value("name").toString();
        s.grade_id = query.value("grade_id").toInt();
        s.stream_id = query.value("stream_id").toInt();
        list.push_back(s);
    }
    return list;
}

QVector<SubjectDetail> SubjectManager::getSubjectsWithDetails() const {
    QVector<SubjectDetail> list;
    QSqlQuery query("SELECT s.*, g.name as grade_name, st.name as stream_name "
                   "FROM subjects s "
                   "JOIN grade_levels g ON s.grade_id = g.id "
                   "LEFT JOIN streams st ON s.stream_id = st.id");
    while (query.next()) {
        SubjectDetail d;
        d.id = query.value("id").toInt();
        d.name = query.value("name").toString();
        d.gradeName = query.value("grade_name").toString();
        d.streamName = query.value("stream_name").toString();
        list.push_back(d);
    }
    return list;
}

QVector<Subject> SubjectManager::getSubjectsForSection(int sectionId) const {
    QVector<Subject> list;
    QSqlQuery query;
    query.prepare("SELECT sub.id, sub.name, sub.grade_id, sub.stream_id FROM subjects sub "
                  "JOIN sections sec ON sub.grade_id = sec.grade_id "
                  "WHERE sec.id = ?");
    query.addBindValue(sectionId);
    if (query.exec()) {
        while (query.next()) {
            Subject s;
            s.id = query.value("id").toInt();
            s.name = query.value("name").toString();
            s.grade_id = query.value("grade_id").toInt();
            s.stream_id = query.value("stream_id").toInt();
            list.push_back(s);
        }
    }
    return list;
}

bool SubjectManager::addSubject(const QString &name, int gradeId, int streamId) {
    QSqlQuery query;
    query.prepare("INSERT INTO subjects (name, grade_id, stream_id) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(gradeId);
    query.addBindValue(streamId > 0 ? QVariant(streamId) : QVariant(QVariant::Int));
    return query.exec();
}

QVector<Subject> SubjectManager::getSubjectsByGrade(int gradeId, int streamId) const {
    QVector<Subject> list;
    QSqlQuery query;
    // Get subjects for the specific grade, filtered by stream or common (stream_id IS NULL)
    QString sql = "SELECT * FROM subjects WHERE grade_id = ? AND (stream_id IS NULL OR stream_id = 0";
    if (streamId > 0) {
        sql += " OR stream_id = ?)";
    } else {
        sql += ")";
    }
    
    query.prepare(sql);
    query.addBindValue(gradeId);
    if (streamId > 0) query.addBindValue(streamId);
    
    if (query.exec()) {
        while (query.next()) {
            Subject s;
            s.id = query.value("id").toInt();
            s.name = query.value("name").toString();
            s.grade_id = query.value("grade_id").toInt();
            s.stream_id = query.value("stream_id").toInt();
            list.push_back(s);
        }
    }
    return list;
}

bool SubjectManager::deleteSubject(int id) {
    // Cascade: remove related data first
    QSqlQuery q;
    q.prepare("DELETE FROM marks WHERE subject_id = ?");
    q.addBindValue(id); q.exec();

    q.prepare("DELETE FROM teaching_assignments WHERE subject_id = ?");
    q.addBindValue(id); q.exec();

    // Unlink teachers specialized in this subject
    q.prepare("UPDATE teachers SET subject_id = NULL WHERE subject_id = ?");
    q.addBindValue(id); q.exec();

    q.prepare("DELETE FROM subjects WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}

bool SubjectManager::assignTeacherToSubject(int teacherId, int subjectId, int sectionId, int yearId) {
    // Rule 1: One teacher can only teach one specific subject (their specialization)
    QSqlQuery checkSpec;
    checkSpec.prepare("SELECT subject_id FROM teachers WHERE id = ?");
    checkSpec.addBindValue(teacherId);
    if (checkSpec.exec() && checkSpec.next()) {
        int specId = checkSpec.value(0).toInt();
        if (specId != subjectId) return false; // Rule violation
    }

    // Rule 3: Only one teacher per subject per section (handled by UNIQUE constraint in DB)
    QSqlQuery query;
    query.prepare("INSERT INTO teaching_assignments (teacher_id, subject_id, section_id, year_id) VALUES (?, ?, ?, ?)");
    query.addBindValue(teacherId);
    query.addBindValue(subjectId);
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    return query.exec();
}
