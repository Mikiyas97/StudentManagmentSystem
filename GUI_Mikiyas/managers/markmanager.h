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

class MarkManager {
public:
    MarkManager();
    
    bool setMark(int studentId, int subjectId, int sectionId, int yearId, int semester, double score);
    double getStudentAverage(int studentId, int yearId, int semester) const;
    QVector<RankInfo> calculateSectionRanking(int sectionId, int yearId, int semester) const;
    bool isRankingApproved(int sectionId, int yearId, int semester) const;
};

#endif // MARKMANAGER_H
