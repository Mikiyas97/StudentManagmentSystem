#include "gradepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>

GradePage::GradePage(const QString &role, int id, QWidget *parent)
    : QWidget(parent), userRole(role), userId(id)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Class Ranking");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    // --- Controls ---
    QHBoxLayout *controls = new QHBoxLayout;
    
    // Grade Filter
    gradeCombo = new QComboBox;
    QSqlQuery gq("SELECT id, name FROM grade_levels ORDER BY CAST(name AS INTEGER) ASC");
    while (gq.next()) gradeCombo->addItem(gq.value("name").toString(), gq.value("id").toInt());
    controls->addWidget(new QLabel("Grade:"));
    controls->addWidget(gradeCombo);

    // Stream Filter (enabled only for Grade 11/12)
    streamCombo = new QComboBox;
    streamCombo->addItem("General", 0);
    QSqlQuery stq("SELECT id, name FROM streams");
    while (stq.next()) streamCombo->addItem(stq.value("name").toString(), stq.value("id").toInt());
    controls->addWidget(new QLabel("Stream:"));
    controls->addWidget(streamCombo);

    // Section Filter (dynamic based on Grade)
    sectionCombo = new QComboBox;
    controls->addWidget(new QLabel("Section:"));
    controls->addWidget(sectionCombo);

    // Update logic
    auto updateSections = [this]() {
        sectionCombo->clear();
        int gradeId = gradeCombo->currentData().toInt();
        QSqlQuery sq;
        sq.prepare("SELECT id, name FROM sections WHERE grade_id = ?");
        sq.addBindValue(gradeId);
        if (sq.exec()) {
            while (sq.next()) sectionCombo->addItem(sq.value("name").toString(), sq.value("id").toInt());
        }
        
        QString gradeText = gradeCombo->currentText();
        bool needsStream = (gradeText == "11" || gradeText == "12");
        streamCombo->setEnabled(needsStream);
        if (!needsStream) streamCombo->setCurrentIndex(0);
    };
    connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSections);
    updateSections(); // Initial load

    yearCombo = new QComboBox;
    QSqlQuery yq("SELECT id, name FROM academic_years ORDER BY name DESC");
    while (yq.next()) yearCombo->addItem(yq.value("name").toString(), yq.value("id").toInt());
    controls->addWidget(new QLabel("Year:"));
    controls->addWidget(yearCombo);

    semesterCombo = new QComboBox;
    semesterCombo->addItem("Semester 1", 1);
    semesterCombo->addItem("Semester 2", 2);
    controls->addWidget(new QLabel("Semester:"));
    controls->addWidget(semesterCombo);

    QPushButton *calcBtn = new QPushButton("Generate Ranking");
    calcBtn->setStyleSheet("background-color: #3498db; color: white;");
    connect(calcBtn, &QPushButton::clicked, this, &GradePage::onCalculate);
    controls->addWidget(calcBtn);

    QPushButton *approveBtn = new QPushButton("Approve Results");
    approveBtn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
    connect(approveBtn, &QPushButton::clicked, this, &GradePage::onApprove);
    controls->addWidget(approveBtn);
    
    controls->addStretch();
    layout->addLayout(controls);

    // --- Table ---
    table = new QTableWidget;
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Rank", "ID", "Student Name", "Total Score", "Average"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    layout->addWidget(table);

    refreshTable();
}

void GradePage::refreshTable() {
    table->setRowCount(0);
}

void GradePage::onCalculate() {
    table->setRowCount(0);
    int sectionId = sectionCombo->currentData().toInt();
    int yearId = yearCombo->currentData().toInt();
    int semester = semesterCombo->currentData().toInt();

    QVector<RankInfo> rankings = manager.calculateSectionRanking(sectionId, yearId, semester);
    
    for (const auto &info : rankings) {
        int r = table->rowCount();
        table->insertRow(r);
        
        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(info.rank));
        rankItem->setTextAlignment(Qt::AlignCenter);
        if (info.rank == 1) rankItem->setForeground(QColor("#f1c40f")); // Gold
        
        table->setItem(r, 0, rankItem);
        table->setItem(r, 1, new QTableWidgetItem(QString::number(info.studentId)));
        table->setItem(r, 2, new QTableWidgetItem(info.studentName));
        table->setItem(r, 3, new QTableWidgetItem(QString::number(info.totalScore, 'f', 1)));
        table->setItem(r, 4, new QTableWidgetItem(QString::number(info.average, 'f', 2)));
    }
}

void GradePage::onApprove() {
    int sectionId = sectionCombo->currentData().toInt();
    int yearId = yearCombo->currentData().toInt();
    int semester = semesterCombo->currentData().toInt();

    QSqlQuery q;
    q.prepare("INSERT OR REPLACE INTO ranking_approvals (section_id, year_id, semester, is_approved) VALUES (?, ?, ?, 1)");
    q.addBindValue(sectionId);
    q.addBindValue(yearId);
    q.addBindValue(semester);
    
    if (q.exec()) {
        QMessageBox::information(this, "Success", "Ranking has been approved and is now visible to students.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to approve ranking.");
    }
}
