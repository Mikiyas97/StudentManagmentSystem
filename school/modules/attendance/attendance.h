#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>

struct Attendance {
    int studentId;
    std::string date;
    std::string status;
};

class AttendanceManager {
private:
    std::vector<Attendance> attendanceList;
    void loadFromFile();
    void saveToFile();

public:
    AttendanceManager();
    void markAttendance();
    void viewAttendance(int filterId = -1);
};

#endif
