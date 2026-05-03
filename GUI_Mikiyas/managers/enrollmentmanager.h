#ifndef ENROLLMENTMANAGER_H
#define ENROLLMENTMANAGER_H

#include <QString>
#include <QVector>

struct Enrollment {
    int studentId;
    QString courseCode;
};

class EnrollmentManager {
public:
    EnrollmentManager();
    bool enrollStudent(int studentId, const QString &courseCode);
    bool unenrollStudent(int studentId, const QString &courseCode);
    QVector<QString> getCoursesForStudent(int studentId) const;
    QVector<int> getStudentsInCourse(const QString &courseCode) const;
};

#endif // ENROLLMENTMANAGER_H
