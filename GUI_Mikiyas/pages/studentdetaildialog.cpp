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
    setFixedSize(500, 500);

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

    // --- Academic Info ---
    QGroupBox *academicGroup = new QGroupBox("Academic Placement");
    QFormLayout *af = new QFormLayout(academicGroup);
    af->addRow(makeFieldLabel("Grade:"),    makeValue(s.gradeName));
    af->addRow(makeFieldLabel("Section:"),  makeValue(s.sectionName));
    af->addRow(makeFieldLabel("Stream:"),   makeValue(s.streamName.isEmpty() ? "General" : s.streamName));
    main->addWidget(academicGroup);

    // --- Contact Info ---
    QGroupBox *contactGroup = new QGroupBox("Contact Information");
    QFormLayout *cf = new QFormLayout(contactGroup);
    cf->addRow(makeFieldLabel("Phone:"),   makeValue(s.phone));
    cf->addRow(makeFieldLabel("Email:"),   makeValue(s.email));
    main->addWidget(contactGroup);

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
