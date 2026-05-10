#include "auth.h"
#include <iostream>
#include <string>
#include <cstdlib>

UserSession Auth::login() {
    std::string username, password;
    std::cout << "--- Login ---" << std::endl;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    UserSession session;
    session.role = "";
    session.studentId = -1;

    if (username == "admin" && password == "admin123") {
        session.role = "admin";
        return session;
    }
    
    // Check if student login (username is student ID)
    // For simplicity, password for students is 'pass' + ID
    bool isDigits = true;
    for(size_t i = 0; i < username.length(); ++i) {
        if (!isdigit(username[i])) {
            isDigits = false;
            break;
        }
    }

    if (isDigits && !username.empty()) {
        int sid = atoi(username.c_str());
        std::string expectedPass = "pass" + username;
        if (password == expectedPass) {
            session.role = "student";
            session.studentId = sid;
            return session;
        }
    }

    std::cout << "Invalid credentials!" << std::endl;
    return session;
}
