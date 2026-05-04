#include "enrollmentmanager.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

EnrollmentManager::EnrollmentManager() {}

bool EnrollmentManager::enrollStudent(int studentId, int offeringId) {
    QSqlQuery q;
    q.prepare("INSERT INTO enrollments (studentId, offeringId, enrollDate) VALUES (?, ?, ?)");
    q.addBindValue(studentId);
    q.addBindValue(offeringId);
    q.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    return q.exec();
}

bool EnrollmentManager::unenrollStudent(int studentId, int offeringId) {
    QSqlQuery q;
    q.prepare("DELETE FROM enrollments WHERE studentId = ? AND offeringId = ?");
    q.addBindValue(studentId);
    q.addBindValue(offeringId);
    return q.exec();
}

QVector<Enrollment> EnrollmentManager::getEnrollmentsForStudent(int studentId) const {
    QVector<Enrollment> list;
    QSqlQuery q;
    q.prepare("SELECT e.id, e.offeringId, c.courseName FROM enrollments e "
              "JOIN course_offerings o ON e.offeringId = o.id "
              "JOIN courses c ON o.courseCode = c.courseCode "
              "WHERE e.studentId = ?");
    q.addBindValue(studentId);
    if (q.exec()) {
        while (q.next()) {
            Enrollment en;
            en.id = q.value(0).toInt();
            en.studentId = studentId;
            en.offeringId = q.value(1).toInt();
            en.courseName = q.value(2).toString();
            list.push_back(en);
        }
    }
    return list;
}
