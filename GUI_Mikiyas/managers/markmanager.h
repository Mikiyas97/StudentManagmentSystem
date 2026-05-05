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
    
    bool setMark(int studentId, int subjectId, int sectionId, int yearId, double score);
    double getStudentAverage(int studentId, int yearId) const;
    QVector<RankInfo> calculateSectionRanking(int sectionId, int yearId) const;
    bool isRankingApproved(int sectionId, int yearId) const;
};

#endif // MARKMANAGER_H
