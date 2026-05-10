CONFIG += console
CONFIG -= app_bundle
QT += core

CONFIG += c++17
...

SOURCES += main.cpp \
           modules/auth/auth.cpp \
           modules/students/student.cpp \
           modules/grades/grade.cpp \
           modules/attendance/attendance.cpp \
           modules/reports/report.cpp \
           modules/courses/course.cpp \
           modules/enrollment/enrollment.cpp

HEADERS += modules/auth/auth.h \
           modules/students/student.h \
           modules/grades/grade.h \
           modules/attendance/attendance.h \
           modules/reports/report.h \
           modules/courses/course.h \
           modules/enrollment/enrollment.h

target.path = .
INSTALLS += target
