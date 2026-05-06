#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QVector>
#include "sectionmanager.h"
#include "subjectmanager.h"

struct Teacher {
    int id;
    QString fullName;
    QString gender;
    QString dateOfBirth;
    QString phone;
    QString email;
    int subject_id; // Added specialization
};

struct User {
    QString username;
    QString password;
    QString role;
    int relatedId;
};

class UserManager {
public:
    UserManager();
    
    // Users
    bool addUser(const User &u);
    bool changePassword(const QString &username, const QString &newPassword);
    User getUserByUsername(const QString &username) const;
    
    // Teachers
    int generateNextTeacherId() const;
    bool addTeacher(const Teacher &t, const QString &password);
    QVector<Teacher> getTeachers() const;
    QVector<Teacher> filterTeachers(const QString &searchText, const QString &sortBy = "") const;
    Teacher getTeacherById(int id) const;
    bool deleteTeacher(int id);
    
    // Teaching Assignments (Moved from pages)
    QVector<GradeLevel> getTeacherGrades(int teacherId) const;
    QVector<Section> getTeacherSections(int teacherId, int gradeId) const;
    QVector<Subject> getTeacherSubjects(int teacherId, int sectionId) const;
    QVector<Teacher> getTeachersBySubject(int subjectId) const;
};

#endif // USERMANAGER_H
