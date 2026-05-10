#include "enrollment.h"
#include <iostream>
#include <fstream>
#include <sstream>

void EnrollmentManager::enrollStudent() {
    Enrollment e;
    std::cout << "Enter Student ID: ";
    std::cin >> e.studentId;
    std::cout << "Enter Course ID: ";
    std::cin >> e.courseId;
    enrollments.push_back(e);
    saveEnrollments();
}

void EnrollmentManager::viewEnrollments() {
    loadEnrollments();
    std::cout << "\n--- Enrollments ---\n";
    for (size_t i = 0; i < enrollments.size(); ++i) {
        std::cout << "Student: " << enrollments[i].studentId << " -> Course: " << enrollments[i].courseId << "\n";
    }
}

void EnrollmentManager::saveEnrollments() {
    std::ofstream file("enrollments.txt");
    for (size_t i = 0; i < enrollments.size(); ++i) {
        file << enrollments[i].studentId << "," << enrollments[i].courseId << "\n";
    }
}

void EnrollmentManager::loadEnrollments() {
    enrollments.clear();
    std::ifstream file("enrollments.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Enrollment e;
        std::getline(ss, e.studentId, ',');
        std::getline(ss, e.courseId, ',');
        enrollments.push_back(e);
    }
}
