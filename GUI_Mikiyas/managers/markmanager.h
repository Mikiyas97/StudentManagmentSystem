#ifndef MARKMANAGER_H
#define MARKMANAGER_H

#include <QString>
#include <QVector>

struct Mark {
    int id;
    int studentId;
    int subjectId;
    int sectionId;
    int yearId;
    int semester;
    double score;
};

struct RankInfo {
    int studentId;
    QString studentName;
    double totalScore;
    double average;
    int rank;
};

struct StudentMark {
    int semester;
    QString subjectName;
    double score;
};

struct StudentMarkEntry {
    int studentId;
    QString fullName;
    double score;
};

class MarkManager {
public:
    MarkManager();
    
    bool setMark(int studentId, int subjectId, int sectionId, int yearId, int semester, double score);
    double getStudentAverage(int studentId, int yearId, int semester) const;
    QVector<RankInfo> calculateSectionRanking(int sectionId, int yearId, int semester) const;
    bool isRankingApproved(int sectionId, int yearId, int semester) const;
    bool approveRanking(int sectionId, int yearId, int semester);
    
    // Queries moved from pages
    QVector<StudentMark> getStudentMarks(int studentId) const;
    QVector<StudentMarkEntry> getStudentsWithMarks(int sectionId, int subjectId, int semester) const;
    int getLatestYearId() const;
    int getStudentSectionId(int studentId) const;
};

#endif // MARKMANAGER_H
