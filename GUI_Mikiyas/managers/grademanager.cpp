#include "grademanager.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

GradeManager::GradeManager() {
}

bool GradeManager::addGrade(int enrollmentId, float score) {
    QSqlQuery query;
    query.prepare("INSERT INTO grades (enrollmentId, score, grade, gradePoint, remarks) "
                  "VALUES (:eid, :score, :grade, :gp, :remarks)");
    query.bindValue(":eid", enrollmentId);
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

QVector<Grade> GradeManager::getGrades(int studentId, const QString &teacherCourse) const {
    QVector<Grade> grades;
    QSqlQuery query;
    
    // Complex join to get course credits and names along with grades
    QString sql = "SELECT g.*, c.courseName, c.credits FROM grades g "
                  "JOIN enrollments e ON g.enrollmentId = e.id "
                  "JOIN course_offerings o ON e.offeringId = o.id "
                  "JOIN courses c ON o.courseCode = c.courseCode ";
    
    QStringList conditions;
    if (studentId != -1) conditions << "e.studentId = :sid";
    if (!teacherCourse.isEmpty()) conditions << "o.courseCode = :course";

    if (!conditions.isEmpty()) sql += " WHERE " + conditions.join(" AND ");
    
    query.prepare(sql);
    if (studentId != -1) query.bindValue(":sid", studentId);
    if (!teacherCourse.isEmpty()) query.bindValue(":course", teacherCourse);
    
    if (query.exec()) {
        while (query.next()) {
            Grade g;
            g.id = query.value("id").toInt();
            g.enrollmentId = query.value("enrollmentId").toInt();
            g.score = query.value("score").toFloat();
            g.letterGrade = query.value("grade").toString();
            g.gradePoint = query.value("gradePoint").toFloat();
            g.credits = query.value("credits").toFloat();
            g.courseName = query.value("courseName").toString();
            grades.push_back(g);
        }
    }
    return grades;
}
