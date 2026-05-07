#include "reportpage.h"
#include "../managers/reportmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFrame>

static QFrame* makeStatCard(const QString &icon, const QString &label, QLabel *&valueLabel) {
    QFrame *card = new QFrame;
    card->setObjectName("statCard");
    card->setStyleSheet(
        "#statCard { background-color: #16213e; border: 1px solid #1f4068;"
        "border-radius: 12px; padding: 20px; min-width: 180px; }"
    );
    QVBoxLayout *lay = new QVBoxLayout(card);
    lay->setAlignment(Qt::AlignCenter);
    lay->setSpacing(8);

    QLabel *iconLbl = new QLabel(icon);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet("font-size: 36px; background: transparent;");

    valueLabel = new QLabel("0");
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #e94560; background: transparent;");

    QLabel *nameLbl = new QLabel(label);
    nameLbl->setAlignment(Qt::AlignCenter);
    nameLbl->setStyleSheet("font-size: 13px; color: #a0a0b0; background: transparent;");

    lay->addWidget(iconLbl);
    lay->addWidget(valueLabel);
    lay->addWidget(nameLbl);
    return card;
}

ReportPage::ReportPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Dashboard Overview");
    title->setObjectName("pageTitle");
    QLabel *subtitle = new QLabel("Real-time summary of school system statistics");
    subtitle->setObjectName("subtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Stats cards in a grid layout
    QGridLayout *cardsLayout = new QGridLayout;
    cardsLayout->setSpacing(15);

    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x8E\x93"), "Total Students", studentCount), 0, 0);
    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x91\xA4"), "Total Teachers", teacherCount), 0, 1);
    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x93\x9A"), "Subjects", courseCount), 0, 2);
    
    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x8F\xAB"), "Sections", sectionCount), 1, 0);
    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x93\x9D"), "Grade Levels", gradeCount), 1, 1);
    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xE2\x9A\x99\xEF\xB8\x8F"), "Assignments", assignmentCount), 1, 2);

    layout->addLayout(cardsLayout);

    QPushButton *refreshBtn = new QPushButton("Refresh Dashboard");
    refreshBtn->setObjectName("actionButton");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setFixedWidth(200);
    refreshBtn->setStyleSheet(
        "QPushButton { background-color: #e94560; color: white; padding: 10px; border-radius: 5px; font-weight: bold; border: none; }"
        "QPushButton:hover { background-color: #ff5e78; }"
    );
    layout->addWidget(refreshBtn, 0, Qt::AlignCenter);
    layout->addStretch(1);

    connect(refreshBtn, &QPushButton::clicked, this, &ReportPage::onRefresh);
    onRefresh();
}

void ReportPage::onRefresh() {
    studentCount->setText(QString::number(ReportManager::countStudents()));
    gradeCount->setText(QString::number(ReportManager::countGrades()));
    courseCount->setText(QString::number(ReportManager::countCourses()));
    teacherCount->setText(QString::number(ReportManager::countTeachers()));
    sectionCount->setText(QString::number(ReportManager::countSections()));
    assignmentCount->setText(QString::number(ReportManager::countAssignments()));
}
