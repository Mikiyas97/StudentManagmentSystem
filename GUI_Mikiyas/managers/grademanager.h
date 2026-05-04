#ifndef GRADEMANAGER_H
#define GRADEMANAGER_H

#include <QString>
#include <QVector>

struct Grade {
    int id;
    int enrollmentId;
    float score;
    QString letterGrade;
    float gradePoint;
    float credits; // cached from course for GPA calc
    QString courseName; // for display
};

class GradeManager {
public:
    GradeManager();
    bool addGrade(int enrollmentId, float score);
    QVector<Grade> getGrades(int studentId = -1, const QString &teacherCourse = "") const;

private:
    // SQLite backend
};

#endif // GRADEMANAGER_H
