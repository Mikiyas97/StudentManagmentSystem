#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H

#include <QString>
#include <QVector>

struct Student {
    int id;
    QString fullName;
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
    bool hardDeleteStudent(int id);     // removes from file
    void bulkSoftDelete(const QVector<int> &ids);
    void bulkHardDelete(const QVector<int> &ids);
    void bulkAssignClass(const QVector<int> &ids, const QString &cls);

    // Query
    QVector<Student> getStudents() const;
    Student getStudentById(int id) const;
    QVector<Student> getStudentsByCourse(const QString &courseCode) const;

    // Advanced search & filter (combined)
    QVector<Student> filter(const QString &nameOrId,
                            const QString &classFilter,
                            const QString &statusFilter) const;

    // Sorting
    enum SortField { ById, ByName, ByClass };
    void sortBy(SortField field, bool ascending = true);

private:
    SortField currentSortField = ById;
    bool currentSortAscending = true;
    
    // Legacy file IO methods removed
    void loadFromFile();
    void saveToFile();
};

#endif // STUDENTMANAGER_H
