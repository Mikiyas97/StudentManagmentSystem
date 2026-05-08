#include "studentdetaildialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QFrame>

#include <QTableWidget>
#include <QHeaderView>

static QLabel* makeValue(const QString &text) {
    QLabel *lbl = new QLabel(text.isEmpty() ? "—" : text);
    lbl->setStyleSheet("color: #eaeaea; font-size: 14px; background: transparent;");
    lbl->setWordWrap(true);
    return lbl;
}

static QLabel* makeFieldLabel(const QString &text) {
    QLabel *lbl = new QLabel(text);
    lbl->setStyleSheet("color: #a0a0b0; font-size: 12px; font-weight: bold; background: transparent;");
    return lbl;
}

StudentDetailDialog::StudentDetailDialog(const Student &s, QWidget *parent)
    : QDialog(parent), student(s)
{
    setWindowTitle("Student Details — " + s.fullName);
    setFixedSize(500, 750);

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setSpacing(12);
    main->setContentsMargins(20, 20, 20, 20);

    // Header
    QHBoxLayout *header = new QHBoxLayout;
    QLabel *avatar = new QLabel(QString::fromUtf8("\xF0\x9F\x91\xA4"));
    avatar->setStyleSheet("font-size: 48px; background: transparent;");

    QVBoxLayout *headerText = new QVBoxLayout;
    QLabel *nameLabel = new QLabel(s.fullName);
    nameLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #e94560; background: transparent;");
    QLabel *idLabel = new QLabel("ID: " + QString::number(s.id));
    idLabel->setStyleSheet("font-size: 13px; color: #a0a0b0; background: transparent;");

    headerText->addWidget(nameLabel);
    headerText->addWidget(idLabel);
    header->addWidget(avatar);
    header->addLayout(headerText, 1);
    main->addLayout(header);

    // Divider
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #1f4068;");
    line->setFixedHeight(1);
    main->addWidget(line);

    // --- Personal Info ---
    QGroupBox *personalGroup = new QGroupBox("Personal Information");
    QFormLayout *pf = new QFormLayout(personalGroup);
    pf->addRow(makeFieldLabel("Gender:"),        makeValue(s.gender.isEmpty() ? "Not Set" : s.gender));
    pf->addRow(makeFieldLabel("Date of Birth:"), makeValue(s.dateOfBirth.isEmpty() ? "Not Set" : s.dateOfBirth));
    pf->addRow(makeFieldLabel("Phone:"),         makeValue(s.phone));
    pf->addRow(makeFieldLabel("Email:"),         makeValue(s.email));
    main->addWidget(personalGroup);

    // --- Academic Info ---
    QGroupBox *academicGroup = new QGroupBox("Academic Placement");
    QFormLayout *af = new QFormLayout(academicGroup);
    af->addRow(makeFieldLabel("Grade:"),    makeValue(s.gradeName));
    af->addRow(makeFieldLabel("Section:"),  makeValue(s.sectionName));
    af->addRow(makeFieldLabel("Stream:"),   makeValue(s.streamName.isEmpty() ? "General" : s.streamName));
    main->addWidget(academicGroup);

    // --- Performance Summary ---
    QGroupBox *marksGroup = new QGroupBox("Academic Performance (2018)");
    QVBoxLayout *marksLayout = new QVBoxLayout(marksGroup);
    
    QTableWidget *marksTable = new QTableWidget;
    marksTable->setColumnCount(7);
    marksTable->setHorizontalHeaderLabels({"Sem", "Subject", "Mid", "Asgn", "Finl", "Total", "Status"});
    marksTable->horizontalHeader()->setStretchLastSection(true);
    marksTable->setFixedHeight(200);
    marksTable->setStyleSheet("QTableWidget { background-color: #16213e; border-radius: 4px; gridline-color: #1f4068; }"
                              "QHeaderView::section { background-color: #0f3460; color: white; padding: 4px; }");
    
    auto marks = markManager.getStudentMarks(s.id);
    for (const auto& m : marks) {
        int r = marksTable->rowCount();
        marksTable->insertRow(r);
        marksTable->setItem(r, 0, new QTableWidgetItem(QString::number(m.semester)));
        marksTable->setItem(r, 1, new QTableWidgetItem(m.subjectName));
        marksTable->setItem(r, 2, new QTableWidgetItem(QString::number(m.midScore, 'f', 0)));
        marksTable->setItem(r, 3, new QTableWidgetItem(QString::number(m.assignmentScore, 'f', 0)));
        marksTable->setItem(r, 4, new QTableWidgetItem(QString::number(m.finalScore, 'f', 0)));
        marksTable->setItem(r, 5, new QTableWidgetItem(QString::number(m.totalScore, 'f', 1)));
        
        QTableWidgetItem *statusItem = new QTableWidgetItem(m.totalScore >= 40 ? "Pass" : "Fail");
        statusItem->setForeground(m.totalScore >= 40 ? QColor("#2ecc71") : QColor("#e74c3c"));
        marksTable->setItem(r, 6, statusItem);
    }
    marksLayout->addWidget(marksTable);
    main->addWidget(marksGroup);

    // --- Buttons ---
    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addStretch();

    QPushButton *editBtn = new QPushButton("Edit Student");
    connect(editBtn, &QPushButton::clicked, this, [this]() {
        emit editRequested(student.id);
        accept();
    });

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setObjectName("secondaryButton");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    btnRow->addWidget(editBtn);
    btnRow->addWidget(closeBtn);
    main->addLayout(btnRow);
}
