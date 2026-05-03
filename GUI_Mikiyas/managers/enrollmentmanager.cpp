#include "enrollmentmanager.h"
#include <QSqlQuery>
#include <QVariant>

EnrollmentManager::EnrollmentManager() {}

bool EnrollmentManager::enrollStudent(int studentId, const QString &courseCode) {
    QSqlQuery q;
    q.prepare("INSERT INTO enrollments (studentId, courseCode) VALUES (?, ?)");
    q.addBindValue(studentId);
    q.addBindValue(courseCode);
    return q.exec();
}

bool EnrollmentManager::unenrollStudent(int studentId, const QString &courseCode) {
    QSqlQuery q;
    q.prepare("DELETE FROM enrollments WHERE studentId = ? AND courseCode = ?");
    q.addBindValue(studentId);
    q.addBindValue(courseCode);
    return q.exec();
}

QVector<QString> EnrollmentManager::getCoursesForStudent(int studentId) const {
    QVector<QString> courses;
    QSqlQuery q;
    q.prepare("SELECT courseCode FROM enrollments WHERE studentId = ?");
    q.addBindValue(studentId);
    if (q.exec()) {
        while (q.next()) {
            courses.push_back(q.value(0).toString());
        }
    }
    return courses;
}

QVector<int> EnrollmentManager::getStudentsInCourse(const QString &courseCode) const {
    QVector<int> students;
    QSqlQuery q;
    q.prepare("SELECT studentId FROM enrollments WHERE courseCode = ?");
    q.addBindValue(courseCode);
    if (q.exec()) {
        while (q.next()) {
            students.push_back(q.value(0).toInt());
        }
    }
    return students;
}
