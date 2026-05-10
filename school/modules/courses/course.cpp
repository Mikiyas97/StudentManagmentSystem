#include "course.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

void CourseManager::addCourse() {
    Course c;
    std::cout << "Enter Course ID: ";
    std::cin >> c.id;
    std::cout << "Enter Course Name: ";
    std::cin.ignore();
    std::getline(std::cin, c.name);
    std::cout << "Enter Credits: ";
    std::cin >> c.credits;
    courses.push_back(c);
    saveCourses();
}

void CourseManager::viewCourses() {
    loadCourses();
    std::cout << "\n--- Courses ---\n";
    for (size_t i = 0; i < courses.size(); ++i) {
        std::cout << "ID: " << courses[i].id << " | Name: " << courses[i].name << " | Credits: " << courses[i].credits << "\n";
    }
}

void CourseManager::saveCourses() {
    std::ofstream file("courses.txt");
    for (size_t i = 0; i < courses.size(); ++i) {
        file << courses[i].id << "," << courses[i].name << "," << courses[i].credits << "\n";
    }
}

void CourseManager::loadCourses() {
    courses.clear();
    std::ifstream file("courses.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Course c;
        std::string creditsStr;
        std::getline(ss, c.id, ',');
        std::getline(ss, c.name, ',');
        std::getline(ss, creditsStr, ',');
        c.credits = atoi(creditsStr.c_str());
        courses.push_back(c);
    }
}
