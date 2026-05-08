#include "enrollmentpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
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
        gradeCombo = new QComboBox;
        sectionFilter = new QComboBox;
        subjectFilter = new QComboBox;

        if (userRole == "teacher") {
            // Load only assigned grades for this teacher
            auto grades = userManager.getTeacherGrades(userStudentId);
            for (const auto& g : grades) {
                gradeCombo->addItem(g.name, g.id);
            }

            auto updateSections = [this]() {
                sectionFilter->clear();
                int gradeId = gradeCombo->currentData().toInt();
                auto sections = userManager.getTeacherSections(userStudentId, gradeId);
                for (const auto& s : sections) {
                    sectionFilter->addItem(s.name, s.id);
                }
            };
            connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSections);
            updateSections(); // initial load

            // Dynamically update subjects based on selected section
            auto updateSubjects = [this](int) {
                subjectFilter->clear();
                int secId = sectionFilter->currentData().toInt();
                auto subjects = userManager.getTeacherSubjects(userStudentId, secId);
                for (const auto& sub : subjects) {
                    subjectFilter->addItem(sub.name, sub.id);
                }
            };
            connect(sectionFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSubjects);
            updateSubjects(0); // initial load
        } else {
            // Admin: Load all grades, sections and subjects
            auto grades = sectionManager.getAllGrades();
            for (const auto& g : grades) gradeCombo->addItem(g.name, g.id);

            auto updateSections = [this]() {
                sectionFilter->clear();
                int gradeId = gradeCombo->currentData().toInt();
                auto sections = sectionManager.getSectionsByGrade(gradeId);
                for (const auto& s : sections) sectionFilter->addItem(s.name, s.id);
            };
            connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSections);
            updateSections();

            auto subjects = subjectManager.getAllSubjects();
            for (const auto& sub : subjects) subjectFilter->addItem(sub.name, sub.id);
        }

        semesterFilter = new QComboBox;
        semesterFilter->addItem("Semester 1", 1);
        semesterFilter->addItem("Semester 2", 2);

        filterBar->addWidget(new QLabel("Grade:"));
        filterBar->addWidget(gradeCombo);
        filterBar->addWidget(new QLabel("Section:"));
        filterBar->addWidget(sectionFilter);
        filterBar->addWidget(new QLabel("Subject:"));
        filterBar->addWidget(subjectFilter);
        filterBar->addWidget(new QLabel("Semester:"));
        filterBar->addWidget(semesterFilter);

        QPushButton *loadBtn = new QPushButton("Load Students");
        connect(loadBtn, &QPushButton::clicked, this, &EnrollmentPage::onFilter);
        filterBar->addWidget(loadBtn);

        QPushButton *saveBtn = new QPushButton("Save All Changes");
        saveBtn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
        connect(saveBtn, &QPushButton::clicked, this, &EnrollmentPage::onSaveAll);
        filterBar->addWidget(saveBtn);
        
        QPushButton *approveBtn = new QPushButton("Approve Marks");
        approveBtn->setStyleSheet("background-color: #3498db; color: white; font-weight: bold;");
        connect(approveBtn, &QPushButton::clicked, this, &EnrollmentPage::onApproveMarks);
        filterBar->addWidget(approveBtn);
        
        filterBar->addStretch();
        layout->addLayout(filterBar);
    }

    // --- Table ---
    table = new QTableWidget;
    if (userRole == "student") {
        table->setColumnCount(7);
        table->setHorizontalHeaderLabels({"Semester", "Subject", "Mid (30)", "Assign (20)", "Final (50)", "Total", "Result"});
    } else {
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"Student ID", "Full Name", "Mid (30)", "Assign (20)", "Final (50)"});
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
        double total = 0;
        int count = 0;
        auto marks = manager.getStudentMarks(userStudentId);
        for (const auto& m : marks) {
            int r = table->rowCount();
            table->insertRow(r);
            table->setItem(r, 0, new QTableWidgetItem(QString::number(m.semester)));
            table->setItem(r, 1, new QTableWidgetItem(m.subjectName));
            table->setItem(r, 2, new QTableWidgetItem(QString::number(m.midScore, 'f', 1)));
            table->setItem(r, 3, new QTableWidgetItem(QString::number(m.assignmentScore, 'f', 1)));
            table->setItem(r, 4, new QTableWidgetItem(QString::number(m.finalScore, 'f', 1)));
            table->setItem(r, 5, new QTableWidgetItem(QString::number(m.totalScore, 'f', 1)));
            table->setItem(r, 6, new QTableWidgetItem(m.totalScore >= 40 ? "Pass" : "Fail"));
            total += m.totalScore;
            count++;
        }
        // --- Rank Calculation Logic ---
        int sectionId = manager.getStudentSectionId(userStudentId);
        int yearId = sectionManager.getSectionYear(sectionId);

        // Let's assume we want the rank for the latest semester (Semester 1 for now)
        int currentSemester = 1;

        QString rankStr = "Not Ranked (Pending)";
        if (manager.isRankingApproved(sectionId, yearId, currentSemester)) {
            QVector<RankInfo> rankings = manager.calculateSectionRanking(sectionId, yearId, currentSemester);
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
    int semester = semesterFilter->currentData().toInt();

    auto students = manager.getStudentsWithMarks(sectionId, subjectId, semester);
    for (const auto& student : students) {
        int r = table->rowCount();
        table->insertRow(r);
        table->setItem(r, 0, new QTableWidgetItem(QString::number(student.studentId)));
        table->setItem(r, 1, new QTableWidgetItem(student.fullName));
        
        QDoubleSpinBox *midSpin = new QDoubleSpinBox;
        midSpin->setRange(0, 30);
        midSpin->setValue(student.midScore);
        table->setCellWidget(r, 2, midSpin);

        QDoubleSpinBox *assignSpin = new QDoubleSpinBox;
        assignSpin->setRange(0, 20);
        assignSpin->setValue(student.assignmentScore);
        table->setCellWidget(r, 3, assignSpin);

        QDoubleSpinBox *finalSpin = new QDoubleSpinBox;
        finalSpin->setRange(0, 50);
        finalSpin->setValue(student.finalScore);
        table->setCellWidget(r, 4, finalSpin);
    }
}

void EnrollmentPage::onSaveAll() {
    int sectionId = sectionFilter->currentData().toInt();
    int subjectId = subjectFilter->currentData().toInt();
    int semester = semesterFilter->currentData().toInt();
    int yearId = sectionManager.getSectionYear(sectionId);

    for (int i = 0; i < table->rowCount(); ++i) {
        int sid = table->item(i, 0)->text().toInt();
        QDoubleSpinBox *midSpin = qobject_cast<QDoubleSpinBox*>(table->cellWidget(i, 2));
        QDoubleSpinBox *assignSpin = qobject_cast<QDoubleSpinBox*>(table->cellWidget(i, 3));
        QDoubleSpinBox *finalSpin = qobject_cast<QDoubleSpinBox*>(table->cellWidget(i, 4));
        
        if (midSpin && assignSpin && finalSpin) {
            manager.setMark(sid, subjectId, sectionId, yearId, semester, 
                            midSpin->value(), assignSpin->value(), finalSpin->value());
        }
    }
    QMessageBox::information(this, "Success", "All student marks have been saved.");
}

void EnrollmentPage::onApproveMarks() {
    int sectionId = sectionFilter->currentData().toInt();
    int subjectId = subjectFilter->currentData().toInt();
    int semester = semesterFilter->currentData().toInt();

    if (sectionId <= 0 || subjectId <= 0) {
        QMessageBox::warning(this, "Error", "Please select a valid section and subject first.");
        return;
    }

    if (manager.approveSubjectMarks(sectionId, subjectId, semester)) {
        QMessageBox::information(this, "Success", "Marks approved successfully! Students can now see their results.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to approve marks.");
    }
}
