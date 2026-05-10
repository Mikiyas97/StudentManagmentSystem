#include <iostream>
#include "modules/auth/auth.h"
#include "modules/students/student.h"
#include "modules/grades/grade.h"
#include "modules/attendance/attendance.h"
#include "modules/reports/report.h"
#include "modules/courses/course.h"
#include "modules/enrollment/enrollment.h"

int main() {
    UserSession session = Auth::login();
    if (session.role == "") return 0;

    StudentManager sm;
    GradeManager gm;
    AttendanceManager am;
    CourseManager cm;
    EnrollmentManager em;
    int choice;
    while (true) {
        std::cout << "\nLogged in as: " << session.role;
        if (session.role == "student") std::cout << " (ID: " << session.studentId << ")";
        std::cout << "\n";

        if (session.role == "admin") {
            std::cout << "1. Add Student\n2. View Students\n3. Add Grade\n4. View Grades\n5. Mark Attendance\n6. View Attendance\n7. Generate Report\n8. Add Course\n9. View Courses\n10. Enroll Student\n11. View Enrollments\n12. Exit\nChoice: ";
        } else {
            std::cout << "1. View My Grades\n2. View My Attendance\n3. Exit\nChoice: ";
        }
        
        std::cin >> choice;
        if (session.role == "admin") {
            if (choice == 1) sm.addStudent();
            else if (choice == 2) sm.viewStudents();
            else if (choice == 3) gm.addGrade();
            else if (choice == 4) gm.viewGrades();
            else if (choice == 5) am.markAttendance();
            else if (choice == 6) am.viewAttendance();
            else if (choice == 7) ReportManager::generateSummary();
            else if (choice == 8) cm.addCourse();
            else if (choice == 9) cm.viewCourses();
            else if (choice == 10) em.enrollStudent();
            else if (choice == 11) em.viewEnrollments();
            else break;
        } else {
            if (choice == 1) gm.viewGrades(session.studentId);
            else if (choice == 2) am.viewAttendance(session.studentId);
            else break;
        }
    }
    return 0;
}
