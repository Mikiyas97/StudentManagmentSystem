#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include <string>
#include <vector>

struct Enrollment {
    std::string studentId;
    std::string courseId;
};

class EnrollmentManager {
public:
    void enrollStudent();
    void viewEnrollments();
    void loadEnrollments();
    void saveEnrollments();
private:
    std::vector<Enrollment> enrollments;
};

#endif
