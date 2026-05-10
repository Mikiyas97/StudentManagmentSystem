cmake_minimum_required(VERSION 3.16)

project(SchoolGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

# Add source files
set(PROJECT_SOURCES
    main.cpp
    ../modules/auth/auth.cpp
    ../modules/students/student.cpp
    ../modules/grades/grade.cpp
    ../modules/attendance/attendance.cpp
    ../modules/reports/report.cpp
    ../modules/courses/course.cpp
    ../modules/enrollment/enrollment.cpp
)

qt_add_executable(school_gui ${PROJECT_SOURCES})

# This MUST create the SchoolGUI URI correctly
qt_add_qml_module(school_gui
    URI SchoolGUI
    VERSION 1.0
    QML_FILES main.qml
)

target_link_libraries(school_gui PRIVATE Qt6::Quick)
