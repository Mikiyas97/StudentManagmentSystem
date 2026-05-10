#include "studentdetaildialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QFrame>

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
    setFixedSize(540, 650);

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

    // Status badge
    QLabel *statusBadge = new QLabel(s.status);
    QString badgeColor = "#2ecc71";
    if (s.status == "Inactive") badgeColor = "#e74c3c";
    else if (s.status == "Graduated") badgeColor = "#3498db";
    statusBadge->setStyleSheet(
        "background-color: " + badgeColor + "; color: white; padding: 3px 12px;"
        "border-radius: 10px; font-size: 11px; font-weight: bold;"
    );
    statusBadge->setFixedHeight(22);

    QHBoxLayout *statusRow = new QHBoxLayout;
    statusRow->addWidget(idLabel);
    statusRow->addWidget(statusBadge);
    statusRow->addStretch();

    headerText->addWidget(nameLabel);
    headerText->addLayout(statusRow);
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
    pf->addRow(makeFieldLabel("Age:"),    makeValue(s.age > 0 ? QString::number(s.age) : ""));
    pf->addRow(makeFieldLabel("Gender:"), makeValue(s.gender));
    pf->addRow(makeFieldLabel("Class:"),  makeValue(s.className));
    main->addWidget(personalGroup);

    // --- Contact Info ---
    QGroupBox *contactGroup = new QGroupBox("Contact Information");
    QFormLayout *cf = new QFormLayout(contactGroup);
    cf->addRow(makeFieldLabel("Phone:"),   makeValue(s.phone));
    cf->addRow(makeFieldLabel("Email:"),   makeValue(s.email));
    cf->addRow(makeFieldLabel("Address:"), makeValue(s.address));
    main->addWidget(contactGroup);

    // --- Guardian Info ---
    QGroupBox *guardGroup = new QGroupBox("Guardian Information");
    QFormLayout *gf = new QFormLayout(guardGroup);
    gf->addRow(makeFieldLabel("Guardian:"),       makeValue(s.guardianName));
    gf->addRow(makeFieldLabel("Guardian Phone:"), makeValue(s.guardianContact));
    main->addWidget(guardGroup);

    // --- Placeholder sections ---
    QGroupBox *academicGroup = new QGroupBox("Academic Summary");
    QVBoxLayout *al = new QVBoxLayout(academicGroup);
    QLabel *academicPlaceholder = new QLabel("Academic data will appear here once grades are linked.");
    academicPlaceholder->setStyleSheet("color: #555; font-style: italic; background: transparent;");
    al->addWidget(academicPlaceholder);
    main->addWidget(academicGroup);

    // --- Buttons ---
    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addStretch();

    QPushButton *editBtn = new QPushButton("Edit Student");
    editBtn->setCursor(Qt::PointingHandCursor);
    connect(editBtn, &QPushButton::clicked, this, [this]() {
        emit editRequested(student.id);
        accept();
    });

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setObjectName("secondaryButton");
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    btnRow->addWidget(editBtn);
    btnRow->addWidget(closeBtn);
    main->addLayout(btnRow);
}
