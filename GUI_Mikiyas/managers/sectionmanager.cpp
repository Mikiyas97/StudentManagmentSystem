#include "sectionmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

SectionManager::SectionManager() {}

bool SectionManager::addSection(int gradeId, const QString &name, int yearId) {
    QSqlQuery query;
    query.prepare("INSERT INTO sections (grade_id, name, year_id) VALUES (?, ?, ?)");
    query.addBindValue(gradeId);
    query.addBindValue(name);
    query.addBindValue(yearId);
    return query.exec();
}

QVector<Section> SectionManager::getSectionsByGrade(int gradeId) const {
    QVector<Section> list;
    QSqlQuery query;
    query.prepare("SELECT s.*, g.name as grade_name, y.name as year_name "
                  "FROM sections s "
                  "JOIN grade_levels g ON s.grade_id = g.id "
                  "JOIN academic_years y ON s.year_id = y.id "
                  "WHERE s.grade_id = ?");
    query.addBindValue(gradeId);
    if (query.exec()) {
        while (query.next()) {
            Section s;
            s.id = query.value("id").toInt();
            s.grade_id = query.value("grade_id").toInt();
            s.gradeName = query.value("grade_name").toString();
            s.name = query.value("name").toString();
            s.year_id = query.value("year_id").toInt();
            s.yearName = query.value("year_name").toString();
            list.push_back(s);
        }
    }
    return list;
}

QVector<Section> SectionManager::getAllSections() const {
    QVector<Section> list;
    QSqlQuery query("SELECT s.*, g.name as grade_name, y.name as year_name "
                   "FROM sections s "
                   "JOIN grade_levels g ON s.grade_id = g.id "
                   "JOIN academic_years y ON s.year_id = y.id");
    while (query.next()) {
        Section s;
        s.id = query.value("id").toInt();
        s.grade_id = query.value("grade_id").toInt();
        s.gradeName = query.value("grade_name").toString();
        s.name = query.value("name").toString();
        s.year_id = query.value("year_id").toInt();
        s.yearName = query.value("year_name").toString();
        list.push_back(s);
    }
    return list;
}

bool SectionManager::deleteSection(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM sections WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}
