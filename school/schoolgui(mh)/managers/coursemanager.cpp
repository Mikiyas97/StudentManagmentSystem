#include "coursemanager.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

CourseManager::CourseManager() {
}

bool CourseManager::addCourse(const QString &id, const QString &name, int credits) {
    QSqlQuery query;
    query.prepare("INSERT INTO courses (code, title, credits) VALUES (:code, :title, :credits)");
    query.bindValue(":code", id);
    query.bindValue(":title", name);
    query.bindValue(":credits", credits);
    return query.exec();
}

QVector<Course> CourseManager::getCourses() {
    QVector<Course> courses;
    QSqlQuery query("SELECT * FROM courses");
    while (query.next()) {
        Course c;
        c.id = query.value("code").toString();
        c.name = query.value("title").toString();
        c.credits = query.value("credits").toInt();
        courses.push_back(c);
    }
    return courses;
}
