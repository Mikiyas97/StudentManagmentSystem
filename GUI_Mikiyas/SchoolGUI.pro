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
           pages/studentpage.cpp \
           pages/studentformdialog.cpp \
           pages/studentdetaildialog.cpp \
           pages/gradepage.cpp \
           pages/coursepage.cpp \
           pages/reportpage.cpp

HEADERS += mainwindow.h \
           logindialog.h \
           managers/studentmanager.h \
           managers/grademanager.h \
           managers/coursemanager.h \
           managers/reportmanager.h \
           pages/studentpage.h \
           pages/studentformdialog.h \
           pages/studentdetaildialog.h \
           pages/gradepage.h \
           pages/coursepage.h \
           pages/reportpage.h
