#ifndef SECTIONMANAGER_H
#define SECTIONMANAGER_H

#include <QString>
#include <QVector>

struct Section {
    int id;
    int grade_id;
    QString gradeName;
    QString name; // A, B, C...
    int year_id;
    QString yearName;
};

class SectionManager {
public:
    SectionManager();
    
    bool addSection(int gradeId, const QString &name, int yearId);
    QVector<Section> getSectionsByGrade(int gradeId) const;
    QVector<Section> getAllSections() const;
    bool deleteSection(int id);
};

#endif // SECTIONMANAGER_H
