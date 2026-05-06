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
        
        filterBar->addStretch();
        layout->addLayout(filterBar);
    }

    // --- Table ---
    table = new QTableWidget;
    if (userRole == "student") {
        table->setColumnCount(4);
        table->setHorizontalHeaderLabels({"Semester", "Subject", "Score (100)", "Result"});
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
        double total = 0;
        int count = 0;
        auto marks = manager.getStudentMarks(userStudentId);
        for (const auto& m : marks) {
            int r = table->rowCount();
            table->insertRow(r);
            table->setItem(r, 0, new QTableWidgetItem(QString::number(m.semester)));
            table->setItem(r, 1, new QTableWidgetItem(m.subjectName));
            table->setItem(r, 2, new QTableWidgetItem(QString::number(m.score, 'f', 1)));
            table->setItem(r, 3, new QTableWidgetItem(m.score >= 40 ? "Pass" : "Fail"));
            total += m.score;
            count++;
        }
        // --- Rank Calculation Logic ---
        int sectionId = manager.getStudentSectionId(userStudentId);
        int yearId = manager.getLatestYearId();

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
        
        QDoubleSpinBox *spin = new QDoubleSpinBox;
        spin->setRange(0, 100);
        spin->setValue(student.score);
        table->setCellWidget(r, 2, spin);
    }
}

void EnrollmentPage::onSaveAll() {
    int sectionId = sectionFilter->currentData().toInt();
    int subjectId = subjectFilter->currentData().toInt();
    int semester = semesterFilter->currentData().toInt();
    int yearId = manager.getLatestYearId();

    for (int i = 0; i < table->rowCount(); ++i) {
        int sid = table->item(i, 0)->text().toInt();
        QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(table->cellWidget(i, 2));
        if (spin) {
            manager.setMark(sid, subjectId, sectionId, yearId, semester, spin->value());
        }
    }
    QMessageBox::information(this, "Success", "All student marks have been saved.");
}
