#ifndef SECTIONMANAGER_H
#define SECTIONMANAGER_H

#include <QString>
#include <QVector>

// --- Shared lookup structs (used by dropdowns across many pages) ---
struct GradeLevel { int id; QString name; };
struct AcademicYear { int id; QString name; };
struct Stream { int id; QString name; };

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
    
    // Lookup helpers (shared across many pages for combo boxes)
    static QVector<GradeLevel> getAllGrades();
    static QVector<AcademicYear> getAllYears();
    static bool addYear(const QString &yearName);
    static QVector<Stream> getAllStreams();
    
    // Section CRUD
    bool addSection(int gradeId, const QString &name, int yearId);
    int getSectionYear(int sectionId) const;
    QVector<Section> getSectionsByGrade(int gradeId) const;
    QVector<Section> getSectionsByGradeAndYear(int gradeId, int yearId) const;
    QVector<Section> getAllSections() const;
    bool deleteSection(int id);
};

#endif // SECTIONMANAGER_H
