#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include <QString>

class ReportManager {
public:
    static int countStudents();
    static int countGrades();
    static int countCourses();
    static int countTeachers();
    static int countSections();
    static int countAssignments();
};

#endif // REPORTMANAGER_H
