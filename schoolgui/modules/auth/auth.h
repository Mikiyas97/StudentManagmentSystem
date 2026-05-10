#ifndef AUTH_H
#define AUTH_H

#include <string>

struct UserSession {
    std::string role;
    int studentId; // -1 for admin/teacher
};

class Auth {
public:
    static UserSession login();
};

#endif
