#include "grade.h"
#include <iostream>
#include <fstream>

GradeManager::GradeManager() { loadFromFile(); }

void GradeManager::loadFromFile() {
    std::ifstream file("grades.txt");
    int sid;
    std::string course;
    float score;
    while (file >> sid >> course >> score) {
        Grade g;
        g.studentId = sid;
        g.course = course;
        g.score = score;
        grades.push_back(g);
    }
}

void GradeManager::saveToFile() {
    std::ofstream file("grades.txt");
    for (size_t i = 0; i < grades.size(); ++i) {
        file << grades[i].studentId << " " << grades[i].course << " " << grades[i].score << std::endl;
    }
}

void GradeManager::addGrade() {
    Grade g;
    std::cout << "Enter Student ID: "; std::cin >> g.studentId;
    std::cout << "Enter Course Name: "; std::cin >> g.course;
    std::cout << "Enter Score: "; std::cin >> g.score;
    grades.push_back(g);
    saveToFile();
    std::cout << "Grade recorded!" << std::endl;
}

void GradeManager::viewGrades(int filterId) {
    bool found = false;
    for (size_t i = 0; i < grades.size(); ++i) {
        if (filterId == -1 || grades[i].studentId == filterId) {
            std::cout << "Student ID: " << grades[i].studentId 
                      << " | Course: " << grades[i].course 
                      << " | Score: " << grades[i].score << std::endl;
            found = true;
        }
    }
    if (!found) {
        std::cout << "No grades found." << std::endl;
    }
}
