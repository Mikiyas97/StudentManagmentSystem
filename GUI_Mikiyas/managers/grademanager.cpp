#include "grademanager.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

GradeManager::GradeManager() {
}

bool GradeManager::addGrade(int studentId, const QString &course, float score) {
    QSqlQuery query;
    query.prepare("INSERT INTO grades (studentId, courseCode, score, grade, remarks) VALUES (:sid, :course, :score, :grade, :remarks)");
    query.bindValue(":sid", studentId);
    query.bindValue(":course", course);
    query.bindValue(":score", score);
    
    // Auto calculate grade string based on score
    QString gradeStr = "F";
    if (score >= 90) gradeStr = "A+";
    else if (score >= 85) gradeStr = "A";
    else if (score >= 80) gradeStr = "A-";
    else if (score >= 75) gradeStr = "B+";
    else if (score >= 70) gradeStr = "B";
    else if (score >= 65) gradeStr = "C+";
    else if (score >= 60) gradeStr = "C";
    else if (score >= 50) gradeStr = "D";
    
    query.bindValue(":grade", gradeStr);
    query.bindValue(":remarks", score >= 60 ? "Pass" : "Fail");
    
    return query.exec();
}

QVector<Grade> GradeManager::getGrades(int filterId) const {
    QVector<Grade> grades;
    QSqlQuery query;
    
    if (filterId == -1) {
        query.prepare("SELECT * FROM grades");
    } else {
        query.prepare("SELECT * FROM grades WHERE studentId = :id");
        query.bindValue(":id", filterId);
    }
    
    if (query.exec()) {
        while (query.next()) {
            Grade g;
            g.studentId = query.value("studentId").toInt();
            g.course = query.value("courseCode").toString();
            g.score = query.value("score").toFloat();
            grades.push_back(g);
        }
    }
    return grades;
}
