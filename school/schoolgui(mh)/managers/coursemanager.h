#ifndef COURSEMANAGER_H
#define COURSEMANAGER_H

#include <QString>
#include <QVector>

struct Course {
    QString id;
    QString name;
    int credits;
};

class CourseManager {
public:
    CourseManager();
    bool addCourse(const QString &id, const QString &name, int credits);
    QVector<Course> getCourses();

private:
    // File I/O removed in favor of SQLite
};

#endif // COURSEMANAGER_H
