#include "enrollmentpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QSqlQuery>
#include <QDoubleSpinBox>
#include <QMessageBox>

EnrollmentPage::EnrollmentPage(const QString &role, int id, QWidget *parent)
    : QWidget(parent), userRole(role), userStudentId(id)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel(userRole == "student" ? "My Academic Results" : "Batch Mark Entry");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    if (userRole != "student") {
        // --- Entry Controls (Admin/Teacher) ---
        QHBoxLayout *filterBar = new QHBoxLayout;
        
        sectionFilter = new QComboBox;
        subjectFilter = new QComboBox;

        if (userRole == "teacher") {
            // Load only assigned sections for this teacher
            QSqlQuery sq;
            sq.prepare("SELECT DISTINCT s.id, s.name FROM sections s "
                       "JOIN teaching_assignments ta ON s.id = ta.section_id "
                       "WHERE ta.teacher_id = ?");
            sq.addBindValue(userStudentId);
            if (sq.exec()) {
                while (sq.next()) sectionFilter->addItem(sq.value("name").toString(), sq.value("id").toInt());
            }

            // Dynamically update subjects based on selected section
            auto updateSubjects = [this](int) {
                subjectFilter->clear();
                int secId = sectionFilter->currentData().toInt();
                QSqlQuery subq;
                subq.prepare("SELECT DISTINCT sub.id, sub.name FROM subjects sub "
                             "JOIN teaching_assignments ta ON sub.id = ta.subject_id "
                             "WHERE ta.teacher_id = ? AND ta.section_id = ?");
                subq.addBindValue(userStudentId);
                subq.addBindValue(secId);
                if (subq.exec()) {
                    while (subq.next()) subjectFilter->addItem(subq.value("name").toString(), subq.value("id").toInt());
                }
            };
            connect(sectionFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSubjects);
            updateSubjects(0); // initial load
        } else {
            // Admin: Load all sections and subjects
            QSqlQuery sq("SELECT id, name FROM sections");
            while (sq.next()) sectionFilter->addItem(sq.value("name").toString(), sq.value("id").toInt());

            QSqlQuery subq("SELECT id, name FROM subjects");
            while (subq.next()) subjectFilter->addItem(subq.value("name").toString(), subq.value("id").toInt());
        }

        filterBar->addWidget(new QLabel("Section:"));
        filterBar->addWidget(sectionFilter);
        filterBar->addWidget(new QLabel("Subject:"));
        filterBar->addWidget(subjectFilter);

        QPushButton *loadBtn = new QPushButton("Load Students");
        connect(loadBtn, &QPushButton::clicked, this, &EnrollmentPage::onFilter);
        filterBar->addWidget(loadBtn);

        QPushButton *saveBtn = new QPushButton("Save All Changes");
        saveBtn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
        connect(saveBtn, &QPushButton::clicked, this, &EnrollmentPage::onSaveAll);
        filterBar->addWidget(saveBtn);
        
        filterBar->addStretch();
        layout->addLayout(filterBar);
    }

    // --- Table ---
    table = new QTableWidget;
    if (userRole == "student") {
        table->setColumnCount(3);
        table->setHorizontalHeaderLabels({"Subject", "Score (100)", "Result"});
    } else {
        table->setColumnCount(3);
        table->setHorizontalHeaderLabels({"Student ID", "Full Name", "Mark (0-100)"});
    }
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(table);

    averageLabel = new QLabel;
    averageLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e94560; margin-top: 10px;");
    layout->addWidget(averageLabel);

    refreshTable();
}

void EnrollmentPage::refreshTable() {
    table->setRowCount(0);
    if (userRole == "student") {
        QSqlQuery query;
        query.prepare("SELECT sub.name, m.score FROM marks m "
                      "JOIN subjects sub ON m.subject_id = sub.id "
                      "WHERE m.student_id = ?");
        query.addBindValue(userStudentId);
        double total = 0;
        int count = 0;
        if (query.exec()) {
            while (query.next()) {
                int r = table->rowCount();
                table->insertRow(r);
                table->setItem(r, 0, new QTableWidgetItem(query.value(0).toString()));
                double score = query.value(1).toDouble();
                table->setItem(r, 1, new QTableWidgetItem(QString::number(score, 'f', 1)));
                table->setItem(r, 2, new QTableWidgetItem(score >= 40 ? "Pass" : "Fail"));
                total += score;
                count++;
            }
        }
        // --- Rank Calculation Logic ---
        int sectionId = 0;
        int yearId = 1; // Default to first year
        QSqlQuery sq;
        sq.prepare("SELECT section_id FROM students WHERE id = ?");
        sq.addBindValue(userStudentId);
        if (sq.exec() && sq.next()) sectionId = sq.value(0).toInt();

        QSqlQuery yq("SELECT id FROM academic_years ORDER BY name DESC LIMIT 1");
        if (yq.next()) yearId = yq.value(0).toInt();

        QString rankStr = "Not Ranked (Pending)";
        if (manager.isRankingApproved(sectionId, yearId)) {
            QVector<RankInfo> rankings = manager.calculateSectionRanking(sectionId, yearId);
            for (const auto &info : rankings) {
                if (info.studentId == userStudentId) {
                    rankStr = QString::number(info.rank);
                    break;
                }
            }
        }

        averageLabel->setText(count > 0 
            ? QString("Total Sum: %1  |  Average: %2  |  Rank: %3")
                .arg(QString::number(total, 'f', 1))
                .arg(QString::number(total/count, 'f', 2))
                .arg(rankStr)
            : "No results published.");
    }
}

void EnrollmentPage::onFilter() {
    if (userRole == "student") return;
    table->setRowCount(0);
    int sectionId = sectionFilter->currentData().toInt();
    int subjectId = subjectFilter->currentData().toInt();

    QSqlQuery query;
    query.prepare("SELECT s.id, s.fullName, (SELECT score FROM marks WHERE student_id = s.id AND subject_id = ?) as score "
                  "FROM students s "
                  "WHERE s.section_id = ?");
    query.addBindValue(subjectId);
    query.addBindValue(sectionId);

    if (query.exec()) {
        while (query.next()) {
            int r = table->rowCount();
            table->insertRow(r);
            table->setItem(r, 0, new QTableWidgetItem(QString::number(query.value(0).toInt())));
            table->setItem(r, 1, new QTableWidgetItem(query.value(1).toString()));
            
            QDoubleSpinBox *spin = new QDoubleSpinBox;
            spin->setRange(0, 100);
            spin->setValue(query.value(2).toDouble());
            table->setCellWidget(r, 2, spin);
        }
    }
}

void EnrollmentPage::onSaveAll() {
    int sectionId = sectionFilter->currentData().toInt();
    int subjectId = subjectFilter->currentData().toInt();
    int yearId = 1;
    QSqlQuery yearQ("SELECT id FROM academic_years ORDER BY name DESC LIMIT 1");
    if (yearQ.next()) yearId = yearQ.value(0).toInt();

    for (int i = 0; i < table->rowCount(); ++i) {
        int sid = table->item(i, 0)->text().toInt();
        QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(table->cellWidget(i, 2));
        if (spin) {
            manager.setMark(sid, subjectId, sectionId, yearId, spin->value());
        }
    }
    QMessageBox::information(this, "Success", "All student marks have been saved.");
}
