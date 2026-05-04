#include "coursemanager.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

CourseManager::CourseManager() {
}

bool CourseManager::addCourse(const Course &c) {
    QSqlQuery query;
    query.prepare("INSERT INTO courses (courseCode, courseName, credits, college) "
                  "VALUES (:code, :name, :credits, :college)");
    query.bindValue(":code", c.code);
    query.bindValue(":name", c.name);
    query.bindValue(":credits", c.credits);
    query.bindValue(":college", c.college);
    return query.exec();
}

QVector<Course> CourseManager::getCourses() {
    QVector<Course> courses;
    QSqlQuery query("SELECT * FROM courses");
    while (query.next()) {
        Course c;
        c.code = query.value("courseCode").toString();
        c.name = query.value("courseName").toString();
        c.credits = query.value("credits").toFloat();
        c.college = query.value("college").toString();
        courses.push_back(c);
    }
    return courses;
}
