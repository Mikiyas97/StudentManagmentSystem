#include "subjectmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

SubjectManager::SubjectManager() {}

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
    QSqlQuery query;
    query.prepare("DELETE FROM subjects WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
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
