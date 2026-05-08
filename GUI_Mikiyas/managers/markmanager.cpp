#include "markmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <algorithm>

MarkManager::MarkManager() {}

bool MarkManager::setMark(int studentId, int subjectId, int sectionId, int yearId, int semester, 
                         double mid, double assignment, double final) {
    double total = mid + assignment + final;
    
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO marks (id, student_id, subject_id, section_id, year_id, semester, mid_score, assignment_score, final_score, total_score) "
                  "VALUES ((SELECT id FROM marks WHERE student_id=? AND subject_id=? AND section_id=? AND year_id=? AND semester=?), "
                  "?, ?, ?, ?, ?, ?, ?, ?, ?)");
                  
    query.addBindValue(studentId);
    query.addBindValue(subjectId);
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    query.addBindValue(semester);
    
    query.addBindValue(studentId);
    query.addBindValue(subjectId);
    query.addBindValue(sectionId);
    query.addBindValue(yearId);
    query.addBindValue(semester);
    
    query.addBindValue(mid);
    query.addBindValue(assignment);
    query.addBindValue(final);
    query.addBindValue(total);
    
    bool ok = query.exec();
    query.finish();
    return ok;
}

double MarkManager::getStudentAverage(int studentId, int yearId, int semester) const {
    QSqlQuery query;
    query.prepare("SELECT AVG(total_score) FROM marks WHERE student_id = ? AND year_id = ? AND semester = ?");
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
    // Join with students to get names, and mark_approvals to only count approved marks
    query.prepare("SELECT m.student_id, s.fullName, SUM(m.total_score) as total, COUNT(m.subject_id) as sub_count "
                  "FROM marks m "
                  "JOIN students s ON m.student_id = s.id "
                  "JOIN mark_approvals ma ON m.section_id = ma.section_id AND m.subject_id = ma.subject_id AND m.semester = ma.semester "
                  "WHERE m.section_id = ? AND m.year_id = ? AND m.semester = ? AND ma.is_approved = 1 "
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

bool MarkManager::approveSubjectMarks(int sectionId, int subjectId, int semester) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO mark_approvals (section_id, subject_id, semester, is_approved) VALUES (?, ?, ?, 1)");
    query.addBindValue(sectionId);
    query.addBindValue(subjectId);
    query.addBindValue(semester);
    return query.exec();
}

bool MarkManager::isSubjectMarksApproved(int sectionId, int subjectId, int semester) const {
    QSqlQuery query;
    query.prepare("SELECT is_approved FROM mark_approvals WHERE section_id = ? AND subject_id = ? AND semester = ?");
    query.addBindValue(sectionId);
    query.addBindValue(subjectId);
    query.addBindValue(semester);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() == 1;
    }
    return false;
}

QVector<StudentMark> MarkManager::getStudentMarks(int studentId) const {
    QVector<StudentMark> list;
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("SELECT m.semester, sub.name, m.mid_score, m.assignment_score, m.final_score, m.total_score FROM marks m "
                  "JOIN subjects sub ON m.subject_id = sub.id "
                  "JOIN mark_approvals ma ON m.section_id = ma.section_id AND m.subject_id = ma.subject_id AND m.semester = ma.semester "
                  "WHERE m.student_id = ? AND ma.is_approved = 1 ORDER BY m.semester ASC, sub.name ASC");
    query.addBindValue(studentId);
    if (query.exec()) {
        while (query.next()) {
            StudentMark sm;
            sm.semester = query.value(0).toInt();
            sm.subjectName = query.value(1).toString();
            sm.midScore = query.value(2).toDouble();
            sm.assignmentScore = query.value(3).toDouble();
            sm.finalScore = query.value(4).toDouble();
            sm.totalScore = query.value(5).toDouble();
            list.push_back(sm);
        }
    }
    query.finish();
    return list;
}

QVector<StudentMarkEntry> MarkManager::getStudentsWithMarks(int sectionId, int subjectId, int semester) const {
    QVector<StudentMarkEntry> list;
    
    // Step 1: Get all students in the section
    {
        QSqlQuery queryStudents;
        queryStudents.setForwardOnly(true);
        queryStudents.prepare("SELECT id, fullName FROM students WHERE section_id = ?");
        queryStudents.addBindValue(sectionId);
        
        if (queryStudents.exec()) {
            while (queryStudents.next()) {
                StudentMarkEntry e;
                e.studentId = queryStudents.value(0).toInt();
                e.fullName = queryStudents.value(1).toString();
                e.midScore = 0.0;
                e.assignmentScore = 0.0;
                e.finalScore = 0.0;
                list.push_back(e);
            }
        }
        queryStudents.finish();
    }

    if (list.isEmpty()) return list;

    // Step 2: Get marks for these students
    {
        QSqlQuery queryMarks;
        queryMarks.setForwardOnly(true);
        queryMarks.prepare("SELECT student_id, mid_score, assignment_score, final_score "
                           "FROM marks WHERE section_id = ? AND subject_id = ? AND semester = ?");
        queryMarks.addBindValue(sectionId);
        queryMarks.addBindValue(subjectId);
        queryMarks.addBindValue(semester);
        
        if (queryMarks.exec()) {
            while (queryMarks.next()) {
                int sId = queryMarks.value(0).toInt();
                for (auto &e : list) {
                    if (e.studentId == sId) {
                        e.midScore = queryMarks.value(1).toDouble();
                        e.assignmentScore = queryMarks.value(2).toDouble();
                        e.finalScore = queryMarks.value(3).toDouble();
                        break;
                    }
                }
            }
        }
        queryMarks.finish();
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
