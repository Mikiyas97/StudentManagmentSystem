#include "reportmanager.h"
#include <QSqlQuery>
#include <QVariant>

static int countTableRows(const QString &tableName) {
    QSqlQuery query("SELECT COUNT(*) FROM " + tableName);
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int ReportManager::countStudents() {
    return countTableRows("students");
}

int ReportManager::countGrades() {
    return countTableRows("grade_levels");
}

int ReportManager::countCourses() {
    return countTableRows("subjects");
}
