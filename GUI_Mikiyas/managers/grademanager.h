#ifndef GRADEMANAGER_H
#define GRADEMANAGER_H

#include <QString>
#include <QVector>

struct Grade {
    int studentId;
    QString course;
    float score;
    QString letterGrade;
    float gradePoint;
};

class GradeManager {
public:
    GradeManager();
    bool addGrade(int studentId, const QString &course, float score);
    QVector<Grade> getGrades(int filterId = -1, const QString &courseFilter = "") const;

private:
    // SQLite backend
};

#endif // GRADEMANAGER_H
