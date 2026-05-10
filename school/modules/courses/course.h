#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <vector>

struct Course {
    std::string id;
    std::string name;
    int credits;
};

class CourseManager {
public:
    void addCourse();
    void viewCourses();
    void loadCourses();
    void saveCourses();
private:
    std::vector<Course> courses;
};

#endif
