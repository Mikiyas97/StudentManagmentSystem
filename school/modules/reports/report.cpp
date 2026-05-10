#include "report.h"
#include <iostream>
#include <fstream>

void ReportManager::generateSummary() {
    std::ifstream sFile("students.txt");
    std::ifstream gFile("grades.txt");
    std::ifstream aFile("attendance.txt");

    int studentCount = 0;
    int id;
    std::string name;
    while (sFile >> id >> name) studentCount++;

    std::cout << "\n--- System Summary Report ---" << std::endl;
    std::cout << "Total Students: " << studentCount << std::endl;
    
    std::cout << "Report generated successfully." << std::endl;
}
