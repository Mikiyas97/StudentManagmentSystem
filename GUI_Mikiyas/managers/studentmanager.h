#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H

#include <QString>
#include <QVector>
#include "linkedlist.h"
#include "stack.h"
#include "queue.h"

struct Student {
    int id;
    QString fullName;
    QString gender;
    QString dateOfBirth;
    int grade_id;
    QString gradeName;
    int section_id;
    QString sectionName;
    int stream_id;
    QString streamName;
    QString phone;
    QString email;
    QString status;
};

class StudentManager {
public:
    StudentManager();

    // CRUD
    int generateNextId() const;
    bool addStudent(const Student &s);
    bool updateStudent(const Student &s);
    bool softDeleteStudent(int id);     // sets status to "Inactive"
    bool hardDeleteStudent(int id);     // removes from database
    void bulkSoftDelete(const QVector<int> &ids);
    void bulkHardDelete(const QVector<int> &ids);
    void bulkAssignClass(const QVector<int> &ids, const QString &cls);
    
    // Stack-based undo feature
    bool undoLastDelete();

    // Query — returns QVector for UI, internally uses LinkedList
    QVector<Student> getStudents() const;
    
    // O(log n) Custom Binary Search
    Student getStudentById(int id) const;

    // Advanced search & filter (combined) — uses LinkedList + merge sort
    QVector<Student> filter(const QString &nameOrId = "",
                           const QString &gradeFilter = "All",
                           const QString &statusFilter = "All",
                           int teacherId = -1) const;

    // Sorting
    enum SortField { ById, ByName, ByClass };
    void sortBy(SortField field, bool ascending = true);

private:
    SortField currentSortField = ById;
    bool currentSortAscending = true;
    
    // Stack to track recently deleted students (full records) for the Undo feature
    Stack<QVector<Student>> undoStack;
    
    // Linked list data loading — loads all students from DB into LinkedList
    LinkedList<Student> loadStudentsAsLinkedList(int teacherId = -1) const;
    
    // Legacy file IO methods removed
    void loadFromFile();
    void saveToFile();
};

#endif // STUDENTMANAGER_H

