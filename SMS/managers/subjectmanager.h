#ifndef SUBJECTMANAGER_H
#define SUBJECTMANAGER_H

#include <QString>
#include <QVector>

struct Subject {
    int id;
    QString name;
    int grade_id;
    int stream_id; // 0 for General
};

struct SubjectDetail {
    int id;
    QString name;
    QString gradeName;
    QString streamName;
};

class SubjectManager {
public:
    SubjectManager();
    
    bool addSubject(const QString &name, int gradeId, int streamId = 0);
    QVector<Subject> getSubjectsByGrade(int gradeId, int streamId = 0) const;
    QVector<Subject> getAllSubjects() const;
    QVector<SubjectDetail> getSubjectsWithDetails() const;
    QVector<Subject> getSubjectsForSection(int sectionId) const;
    QString getSubjectName(int id) const;
    bool deleteSubject(int id);
    bool assignTeacherToSubject(int teacherId, int subjectId, int sectionId, int yearId);
};

#endif // SUBJECTMANAGER_H
