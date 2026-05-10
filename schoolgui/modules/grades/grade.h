#ifndef GRADE_H
#define GRADE_H

#include <string>
#include <vector>

struct Grade {
    int studentId;
    std::string course;
    float score;
};

class GradeManager {
private:
    std::vector<Grade> grades;
    void loadFromFile();
    void saveToFile();

public:
    GradeManager();
    void addGrade();
    void viewGrades(int filterId = -1);
};

#endif
