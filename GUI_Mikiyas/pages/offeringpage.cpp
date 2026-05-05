#include "offeringpage.h"
#include "assignmentdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>

OfferingPage::OfferingPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Manage Sections (Classes)");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    // --- Add Section Section ---
    QWidget *inputArea = new QWidget;
    QHBoxLayout *inputLayout = new QHBoxLayout(inputArea);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    gradeCombo = new QComboBox;
    QSqlQuery gq("SELECT id, name FROM grade_levels ORDER BY CAST(name AS INTEGER) ASC");
    while (gq.next()) gradeCombo->addItem(gq.value("name").toString(), gq.value("id").toInt());
    inputLayout->addWidget(new QLabel("Grade:"));
    inputLayout->addWidget(gradeCombo);

    sectionNameEdit = new QLineEdit;
    sectionNameEdit->setPlaceholderText("Section (e.g. A)");
    sectionNameEdit->setFixedWidth(80);
    inputLayout->addWidget(new QLabel("Section Name:"));
    inputLayout->addWidget(sectionNameEdit);

    yearCombo = new QComboBox;
    QSqlQuery yq("SELECT id, name FROM academic_years ORDER BY name DESC");
    while (yq.next()) yearCombo->addItem(yq.value("name").toString(), yq.value("id").toInt());
    inputLayout->addWidget(new QLabel("Year:"));
    inputLayout->addWidget(yearCombo);

    QPushButton *addBtn = new QPushButton("Create Section");
    connect(addBtn, &QPushButton::clicked, this, &OfferingPage::onAdd);
    inputLayout->addWidget(addBtn);

    QPushButton *assignBtn = new QPushButton("Teacher Assignments");
    assignBtn->setStyleSheet("background-color: #3498db; color: white; margin-left: 10px;");
    connect(assignBtn, &QPushButton::clicked, this, &OfferingPage::onOpenAssignments);
    inputLayout->addWidget(assignBtn);

    layout->addWidget(inputArea);

    // --- Table ---
    table = new QTableWidget;
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Grade", "Section", "Academic Year"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(table);

    refreshTable();
}

void OfferingPage::refreshTable() {
    table->setRowCount(0);
    QVector<Section> sections = manager.getAllSections();
    for (const auto &s : sections) {
        int r = table->rowCount();
        table->insertRow(r);
        table->setItem(r, 0, new QTableWidgetItem(QString::number(s.id)));
        table->setItem(r, 1, new QTableWidgetItem(s.gradeName));
        table->setItem(r, 2, new QTableWidgetItem(s.name));
        table->setItem(r, 3, new QTableWidgetItem(s.yearName));
    }
}

void OfferingPage::onAdd() {
    QString name = sectionNameEdit->text().trimmed();
    if (name.isEmpty()) return;

    if (manager.addSection(gradeCombo->currentData().toInt(), name, yearCombo->currentData().toInt())) {
        sectionNameEdit->clear();
        refreshTable();
    } else {
        QMessageBox::warning(this, "Error", "Failed to create section.");
    }
}

void OfferingPage::onOpenAssignments() {
    AssignmentDialog dlg(this);
    dlg.exec();
}
