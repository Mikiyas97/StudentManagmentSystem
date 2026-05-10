#include "student.h"
#include <iostream>
#include <fstream>

StudentManager::StudentManager() { loadFromFile(); }

void StudentManager::loadFromFile() {
    std::ifstream file("students.txt");
    int id;
    std::string name;
    while (file >> id >> name) {
        Student s;
        s.id = id;
        s.name = name;
        students.push_back(s);
    }
}

void StudentManager::saveToFile() {
    std::ofstream file("students.txt");
    for (size_t i = 0; i < students.size(); ++i) {
        file << students[i].id << " " << students[i].name << std::endl;
    }
}

void StudentManager::addStudent() {
    Student s;
    std::cout << "Enter ID: "; std::cin >> s.id;
    std::cout << "Enter Name: "; std::cin >> s.name;
    students.push_back(s);
    saveToFile();
    std::cout << "Student added!" << std::endl;
}

void StudentManager::viewStudents() {
    for (size_t i = 0; i < students.size(); ++i) {
        std::cout << "ID: " << students[i].id << " | Name: " << students[i].name << std::endl;
    }
}

void StudentManager::updateStudent() {
    int id;
    std::cout << "Enter ID to update: "; std::cin >> id;
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].id == id) {
            std::cout << "Enter New Name: "; std::cin >> students[i].name;
            saveToFile();
            return;
        }
    }
    std::cout << "Student not found." << std::endl;
}

void StudentManager::deleteStudent() {
    int id;
    std::cout << "Enter ID to delete: "; std::cin >> id;
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].id == id) {
            students.erase(students.begin() + i);
            saveToFile();
            return;
        }
    }
}

void StudentManager::searchStudent() {
    int id;
    std::cout << "Enter ID to search: "; std::cin >> id;
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].id == id) {
            std::cout << "Found: ID: " << students[i].id << " | Name: " << students[i].name << std::endl;
            return;
        }
    }
    std::cout << "Student not found." << std::endl;
}

void StudentManager::sortStudents() {
    if (students.empty()) return;
    for (size_t i = 0; i < students.size() - 1; ++i) {
        for (size_t j = 0; j < students.size() - i - 1; ++j) {
            if (students[j].id > students[j + 1].id) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    saveToFile();
    std::cout << "Students sorted by ID." << std::endl;
}
