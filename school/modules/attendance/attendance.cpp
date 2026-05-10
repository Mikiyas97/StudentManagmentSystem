#include "attendance.h"
#include <iostream>
#include <fstream>

AttendanceManager::AttendanceManager() { loadFromFile(); }

void AttendanceManager::loadFromFile() {
    std::ifstream file("attendance.txt");
    int sid;
    std::string date, status;
    while (file >> sid >> date >> status) {
        Attendance a;
        a.studentId = sid;
        a.date = date;
        a.status = status;
        attendanceList.push_back(a);
    }
}

void AttendanceManager::saveToFile() {
    std::ofstream file("attendance.txt");
    for (size_t i = 0; i < attendanceList.size(); ++i) {
        file << attendanceList[i].studentId << " " << attendanceList[i].date << " " << attendanceList[i].status << std::endl;
    }
}

void AttendanceManager::markAttendance() {
    Attendance a;
    std::cout << "Enter Student ID: "; std::cin >> a.studentId;
    std::cout << "Enter Date (YYYY-MM-DD): "; std::cin >> a.date;
    std::cout << "Enter Status (Present/Absent): "; std::cin >> a.status;
    attendanceList.push_back(a);
    saveToFile();
    std::cout << "Attendance marked!" << std::endl;
}

void AttendanceManager::viewAttendance(int filterId) {
    bool found = false;
    for (size_t i = 0; i < attendanceList.size(); ++i) {
        if (filterId == -1 || attendanceList[i].studentId == filterId) {
            std::cout << "Student ID: " << attendanceList[i].studentId 
                      << " | Date: " << attendanceList[i].date 
                      << " | Status: " << attendanceList[i].status << std::endl;
            found = true;
        }
    }
    if (!found) {
        std::cout << "No attendance records found." << std::endl;
    }
}
