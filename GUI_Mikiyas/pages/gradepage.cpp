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
    
    sectionCombo = new QComboBox;
    QSqlQuery sq("SELECT id, name FROM sections");
    while (sq.next()) sectionCombo->addItem(sq.value("name").toString(), sq.value("id").toInt());
    controls->addWidget(new QLabel("Section:"));
    controls->addWidget(sectionCombo);

    yearCombo = new QComboBox;
    QSqlQuery yq("SELECT id, name FROM academic_years");
    while (yq.next()) yearCombo->addItem(yq.value("name").toString(), yq.value("id").toInt());
    controls->addWidget(new QLabel("Year:"));
    controls->addWidget(yearCombo);

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

    QVector<RankInfo> rankings = manager.calculateSectionRanking(sectionId, yearId);
    
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

    QSqlQuery q;
    q.prepare("INSERT OR REPLACE INTO ranking_approvals (section_id, year_id, is_approved) VALUES (?, ?, 1)");
    q.addBindValue(sectionId);
    q.addBindValue(yearId);
    
    if (q.exec()) {
        QMessageBox::information(this, "Success", "Ranking has been approved and is now visible to students.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to approve ranking.");
    }
}
