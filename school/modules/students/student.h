#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

struct Student {
    int id;
    std::string name;
};

class StudentManager {
private:
    std::vector<Student> students;
    void loadFromFile();
    void saveToFile();

public:
    StudentManager();
    void addStudent();
    void viewStudents();
    void updateStudent();
    void deleteStudent();
    void searchStudent();
    void sortStudents();
};

#endif
