#ifndef COURSEMANAGER_H
#define COURSEMANAGER_H

#include <QString>
#include <QVector>

struct Course {
    QString code;
    QString name;
    float credits;
    QString college;
};

class CourseManager {
public:
    CourseManager();
    bool addCourse(const Course &c);
    QVector<Course> getCourses();
};

#endif // COURSEMANAGER_H
