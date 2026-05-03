#include "grademanager.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

GradeManager::GradeManager() {
}

bool GradeManager::addGrade(int studentId, const QString &course, float score) {
    QSqlQuery query;
    query.prepare("INSERT INTO grades (studentId, courseCode, score, grade, gradePoint, remarks) "
                  "VALUES (:sid, :course, :score, :grade, :gp, :remarks)");
    query.bindValue(":sid", studentId);
    query.bindValue(":course", course);
    query.bindValue(":score", score);
    
    QString gradeStr = "F";
    float gp = 0.0;
    
    if (score >= 90) { gradeStr = "A+"; gp = 4.0; }
    else if (score >= 85) { gradeStr = "A"; gp = 4.0; }
    else if (score >= 80) { gradeStr = "A-"; gp = 3.75; }
    else if (score >= 75) { gradeStr = "B+"; gp = 3.5; }
    else if (score >= 70) { gradeStr = "B"; gp = 3.0; }
    else if (score >= 65) { gradeStr = "B-"; gp = 2.75; }
    else if (score >= 60) { gradeStr = "C+"; gp = 2.5; }
    else if (score >= 55) { gradeStr = "C"; gp = 2.0; }
    else if (score >= 50) { gradeStr = "C-"; gp = 1.75; }
    else if (score >= 45) { gradeStr = "D"; gp = 1.0; }
    else { gradeStr = "F"; gp = 0.0; }
    
    query.bindValue(":grade", gradeStr);
    query.bindValue(":gp", gp);
    query.bindValue(":remarks", score >= 45 ? "Pass" : "Fail");
    
    return query.exec();
}

QVector<Grade> GradeManager::getGrades(int filterId, const QString &courseFilter) const {
    QVector<Grade> grades;
    QSqlQuery query;
    
    QString sql = "SELECT * FROM grades";
    QStringList conditions;
    
    if (filterId != -1) conditions << "studentId = :id";
    if (!courseFilter.isEmpty()) conditions << "courseCode = :course";
    
    if (!conditions.isEmpty()) {
        sql += " WHERE " + conditions.join(" AND ");
    }
    
    query.prepare(sql);
    if (filterId != -1) query.bindValue(":id", filterId);
    if (!courseFilter.isEmpty()) query.bindValue(":course", courseFilter);
    
    if (query.exec()) {
        while (query.next()) {
            Grade g;
            g.studentId = query.value("studentId").toInt();
            g.course = query.value("courseCode").toString();
            g.score = query.value("score").toFloat();
            g.letterGrade = query.value("grade").toString();
            g.gradePoint = query.value("gradePoint").toFloat();
            grades.push_back(g);
        }
    }
    return grades;
}
