#ifndef GRADEMANAGER_H
#define GRADEMANAGER_H

#include <QString>
#include <QVector>

struct Grade {
    int studentId;
    QString course;
    float score;
};

class GradeManager {
public:
    GradeManager();
    bool addGrade(int studentId, const QString &course, float score);
    QVector<Grade> getGrades(int filterId = -1) const;

private:
    // SQLite backend
};

#endif // GRADEMANAGER_H
