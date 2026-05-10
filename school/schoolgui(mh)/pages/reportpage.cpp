#include "reportpage.h"
#include "../managers/reportmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
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

    QLabel *title = new QLabel("System Report");
    title->setObjectName("pageTitle");
    QLabel *subtitle = new QLabel("Summary of all system data");
    subtitle->setObjectName("subtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Stats cards in a horizontal layout
    QHBoxLayout *cardsLayout = new QHBoxLayout;
    cardsLayout->setSpacing(15);

    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x91\xA4"), "Students", studentCount));
    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x93\x9D"), "Grades", gradeCount));
    cardsLayout->addWidget(makeStatCard(QString::fromUtf8("\xF0\x9F\x93\x9A"), "Courses", courseCount));

    layout->addLayout(cardsLayout);

    QPushButton *refreshBtn = new QPushButton("Refresh Report");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setFixedWidth(200);
    layout->addWidget(refreshBtn, 0, Qt::AlignCenter);
    layout->addStretch(1);

    connect(refreshBtn, &QPushButton::clicked, this, &ReportPage::onRefresh);
    onRefresh();
}

void ReportPage::onRefresh() {
    studentCount->setText(QString::number(ReportManager::countStudents()));
    gradeCount->setText(QString::number(ReportManager::countGrades()));
    courseCount->setText(QString::number(ReportManager::countCourses()));
}
