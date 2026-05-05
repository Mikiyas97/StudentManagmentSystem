QT       += core gui widgets sql
CONFIG   += c++17
TARGET    = SchoolGUI
TEMPLATE  = app

SOURCES += main.cpp \
           mainwindow.cpp \
           logindialog.cpp \
           managers/studentmanager.cpp \
           managers/grademanager.cpp \
           managers/coursemanager.cpp \
           managers/reportmanager.cpp \
           managers/usermanager.cpp \
           managers/enrollmentmanager.cpp \
           managers/sectionmanager.cpp \
           managers/subjectmanager.cpp \
           managers/markmanager.cpp \
           pages/studentpage.cpp \
           pages/studentformdialog.cpp \
           pages/studentdetaildialog.cpp \
           pages/gradepage.cpp \
           pages/coursepage.cpp \
           pages/reportpage.cpp \
           pages/teacherpage.cpp \
           pages/teacherformdialog.cpp \
           pages/teacherdetaildialog.cpp \
           pages/assignmentdialog.cpp \
           pages/enrollmentpage.cpp \
           pages/offeringpage.cpp \
           pages/profilepage.cpp

HEADERS += mainwindow.h \
           logindialog.h \
           managers/studentmanager.h \
           managers/grademanager.h \
           managers/coursemanager.h \
           managers/reportmanager.h \
           managers/usermanager.h \
           managers/enrollmentmanager.h \
           managers/sectionmanager.h \
           managers/subjectmanager.h \
           managers/markmanager.h \
           pages/studentpage.h \
           pages/studentformdialog.h \
           pages/studentdetaildialog.h \
           pages/gradepage.h \
           pages/coursepage.h \
           pages/reportpage.h \
           pages/teacherpage.h \
           pages/teacherformdialog.h \
           pages/teacherdetaildialog.h \
           pages/assignmentdialog.h \
           pages/enrollmentpage.h \
           pages/offeringpage.h \
           pages/profilepage.h
