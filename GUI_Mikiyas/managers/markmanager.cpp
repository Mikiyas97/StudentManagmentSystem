#include "markmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <algorithm>

MarkManager::MarkManager() {}

bool MarkManager::setMark(int studentId, int subjectId, int sectionId, int yearId, int semester, double score) {
    QSqlQuery del;
    del.prepare("DELETE FROM marks WHERE student_id=? AND subject_id=? AND section_id=? AND year_id=? AND semester=?");
    del.addBindValue(studentId);
    del.addBindValue(subjectId);
    del.addBindValue(sectionId);
    del.addBindValue(yearId);
    del.addBindValue(semester);
    del.exec();

    QSqlQuery query;
    query.prepare("INSERT INTO marks (student_id, subject_id, section_id, year_id, semester, score) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(studentId);
    query.addBindValue(subjectId);
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    query.addBindValue(semester);
    query.addBindValue(score);
    return query.exec();
}

double MarkManager::getStudentAverage(int studentId, int yearId, int semester) const {
    QSqlQuery query;
    query.prepare("SELECT AVG(score) FROM marks WHERE student_id = ? AND year_id = ? AND semester = ?");
    query.addBindValue(studentId);
    query.addBindValue(yearId);
    query.addBindValue(semester);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

QVector<RankInfo> MarkManager::calculateSectionRanking(int sectionId, int yearId, int semester) const {
    QVector<RankInfo> ranking;
    QSqlQuery query;
    // Join with students to get names
    query.prepare("SELECT m.student_id, s.fullName, SUM(m.score) as total, COUNT(m.subject_id) as sub_count "
                  "FROM marks m "
                  "JOIN students s ON m.student_id = s.id "
                  "WHERE m.section_id = ? AND m.year_id = ? AND m.semester = ? "
                  "GROUP BY m.student_id "
                  "ORDER BY total DESC");
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    query.addBindValue(semester);
    
    if (query.exec()) {
        int r = 1;
        while (query.next()) {
            RankInfo info;
            info.studentId = query.value(0).toInt();
            info.studentName = query.value(1).toString();
            info.totalScore = query.value(2).toDouble();
            int counts = query.value(3).toInt();
            info.average = (counts > 0) ? (info.totalScore / counts) : 0;
            info.rank = r++;
            ranking.push_back(info);
        }
    }
    return ranking;
}

bool MarkManager::isRankingApproved(int sectionId, int yearId, int semester) const {
    QSqlQuery query;
    query.prepare("SELECT is_approved FROM ranking_approvals WHERE section_id = ? AND year_id = ? AND semester = ?");
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    query.addBindValue(semester);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() == 1;
    }
    return false;
}

bool MarkManager::approveRanking(int sectionId, int yearId, int semester) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO ranking_approvals (section_id, year_id, semester, is_approved) VALUES (?, ?, ?, 1)");
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    query.addBindValue(semester);
    return query.exec();
}

QVector<StudentMark> MarkManager::getStudentMarks(int studentId) const {
    QVector<StudentMark> list;
    QSqlQuery query;
    query.prepare("SELECT m.semester, sub.name, m.score FROM marks m "
                  "JOIN subjects sub ON m.subject_id = sub.id "
                  "WHERE m.student_id = ? ORDER BY m.semester ASC, sub.name ASC");
    query.addBindValue(studentId);
    if (query.exec()) {
        while (query.next()) {
            StudentMark sm;
            sm.semester = query.value(0).toInt();
            sm.subjectName = query.value(1).toString();
            sm.score = query.value(2).toDouble();
            list.push_back(sm);
        }
    }
    return list;
}

QVector<StudentMarkEntry> MarkManager::getStudentsWithMarks(int sectionId, int subjectId, int semester) const {
    QVector<StudentMarkEntry> list;
    QSqlQuery query;
    query.prepare("SELECT s.id, s.fullName, (SELECT score FROM marks WHERE student_id = s.id AND subject_id = ? AND semester = ?) as score "
                  "FROM students s "
                  "WHERE s.section_id = ?");
    query.addBindValue(subjectId);
    query.addBindValue(semester);
    query.addBindValue(sectionId);

    if (query.exec()) {
        while (query.next()) {
            StudentMarkEntry e;
            e.studentId = query.value(0).toInt();
            e.fullName = query.value(1).toString();
            e.score = query.value(2).toDouble(); // will be 0 if no score
            list.push_back(e);
        }
    }
    return list;
}

int MarkManager::getLatestYearId() const {
    QSqlQuery yq("SELECT id FROM academic_years ORDER BY name DESC LIMIT 1");
    if (yq.next()) {
        return yq.value(0).toInt();
    }
    return 1; // Default
}

int MarkManager::getStudentSectionId(int studentId) const {
    QSqlQuery sq;
    sq.prepare("SELECT section_id FROM students WHERE id = ?");
    sq.addBindValue(studentId);
    if (sq.exec() && sq.next()) {
        return sq.value(0).toInt();
    }
    return 0;
}
