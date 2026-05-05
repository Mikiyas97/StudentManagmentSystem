#include "markmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <algorithm>

MarkManager::MarkManager() {}

bool MarkManager::setMark(int studentId, int subjectId, int sectionId, int yearId, double score) {
    QSqlQuery query;
    // Use INSERT OR REPLACE (UPSERT logic)
    query.prepare("INSERT INTO marks (student_id, subject_id, section_id, year_id, score) "
                  "VALUES (?, ?, ?, ?, ?) "
                  "ON CONFLICT(student_id, subject_id, section_id, year_id) DO UPDATE SET score=excluded.score");
    // SQLite ON CONFLICT requires a UNIQUE constraint. Let's make sure we have one or use a manual check.
    // For simplicity, let's use a DELETE then INSERT approach if not using a specific UNIQUE constraint.
    
    QSqlQuery del;
    del.prepare("DELETE FROM marks WHERE student_id=? AND subject_id=? AND section_id=? AND year_id=?");
    del.addBindValue(studentId);
    del.addBindValue(subjectId);
    del.addBindValue(sectionId);
    del.addBindValue(yearId);
    del.exec();

    query.prepare("INSERT INTO marks (student_id, subject_id, section_id, year_id, score) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(studentId);
    query.addBindValue(subjectId);
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    query.addBindValue(score);
    return query.exec();
}

double MarkManager::getStudentAverage(int studentId, int yearId) const {
    QSqlQuery query;
    query.prepare("SELECT AVG(score) FROM marks WHERE student_id = ? AND year_id = ?");
    query.addBindValue(studentId);
    query.addBindValue(yearId);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

QVector<RankInfo> MarkManager::calculateSectionRanking(int sectionId, int yearId) const {
    QVector<RankInfo> ranking;
    QSqlQuery query;
    // Join with students to get names
    query.prepare("SELECT student_id, s.fullName, SUM(score) as total, COUNT(subject_id) as sub_count "
                  "FROM marks m "
                  "JOIN students s ON m.student_id = s.id "
                  "WHERE m.section_id = ? AND m.year_id = ? "
                  "GROUP BY student_id "
                  "ORDER BY total DESC");
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    
    if (query.exec()) {
        int r = 1;
        while (query.next()) {
            RankInfo info;
            info.studentId = query.value("student_id").toInt();
            info.studentName = query.value("fullName").toString();
            info.totalScore = query.value("total").toDouble();
            int counts = query.value("sub_count").toInt();
            info.average = (counts > 0) ? (info.totalScore / counts) : 0;
            info.rank = r++;
            ranking.push_back(info);
        }
    }
    return ranking;
}

bool MarkManager::isRankingApproved(int sectionId, int yearId) const {
    QSqlQuery query;
    query.prepare("SELECT is_approved FROM ranking_approvals WHERE section_id = ? AND year_id = ?");
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() == 1;
    }
    return false;
}
