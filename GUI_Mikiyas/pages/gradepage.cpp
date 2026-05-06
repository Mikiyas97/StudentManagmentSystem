#include "gradepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
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
    auto grades = sectionManager.getAllGrades();
    for (const auto& g : grades) gradeCombo->addItem(g.name, g.id);
    controls->addWidget(new QLabel("Grade:"));
    controls->addWidget(gradeCombo);

    // Stream Filter (enabled only for Grade 11/12)
    streamCombo = new QComboBox;
    streamCombo->addItem("General", 0);
    auto streams = sectionManager.getAllStreams();
    for (const auto& st : streams) streamCombo->addItem(st.name, st.id);
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
        auto sections = sectionManager.getSectionsByGrade(gradeId);
        for (const auto& s : sections) sectionCombo->addItem(s.name, s.id);
        
        QString gradeText = gradeCombo->currentText();
        bool needsStream = (gradeText == "11" || gradeText == "12");
        streamCombo->setEnabled(needsStream);
        if (!needsStream) streamCombo->setCurrentIndex(0);
    };
    connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSections);
    updateSections(); // Initial load

    yearCombo = new QComboBox;
    auto years = sectionManager.getAllYears();
    for (const auto& y : years) yearCombo->addItem(y.name, y.id);
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

    if (manager.approveRanking(sectionId, yearId, semester)) {
        QMessageBox::information(this, "Success", "Ranking has been approved and is now visible to students.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to approve ranking.");
    }
}
