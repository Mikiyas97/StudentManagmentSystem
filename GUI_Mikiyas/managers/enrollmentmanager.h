#ifndef ENROLLMENTMANAGER_H
#define ENROLLMENTMANAGER_H

#include <QString>
#include <QVector>

struct Enrollment {
    int id;
    int studentId;
    int offeringId;
    QString courseName; // for display
};

class EnrollmentManager {
public:
    EnrollmentManager();
    bool enrollStudent(int studentId, int offeringId);
    bool unenrollStudent(int studentId, int offeringId);
    QVector<Enrollment> getEnrollmentsForStudent(int studentId) const;
};

#endif // ENROLLMENTMANAGER_H
