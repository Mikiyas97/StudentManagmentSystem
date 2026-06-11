#include "offeringpage.h"
#include "assignmentdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QInputDialog>

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
    auto grades = manager.getAllGrades();
    for (const auto& g : grades) gradeCombo->addItem(g.name, g.id);
    inputLayout->addWidget(new QLabel("Grade:"));
    inputLayout->addWidget(gradeCombo);

    sectionNameEdit = new QLineEdit;
    sectionNameEdit->setPlaceholderText("Section (e.g. A)");
    sectionNameEdit->setFixedWidth(80);
    inputLayout->addWidget(new QLabel("Section Name:"));
    inputLayout->addWidget(sectionNameEdit);

    yearCombo = new QComboBox;
    auto years = manager.getAllYears();
    for (const auto& y : years) yearCombo->addItem(y.name, y.id);
    inputLayout->addWidget(new QLabel("Year:"));
    inputLayout->addWidget(yearCombo);

    QPushButton *addYearBtn = new QPushButton("+");
    addYearBtn->setFixedWidth(30);
    connect(addYearBtn, &QPushButton::clicked, this, &OfferingPage::onAddYear);
    inputLayout->addWidget(addYearBtn);

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

void OfferingPage::onAddYear() {
    bool ok;
    QString yearName = QInputDialog::getText(this, "Add Academic Year", "Enter Year (e.g. 2018):", QLineEdit::Normal, "", &ok);
    if (ok && !yearName.trimmed().isEmpty()) {
        if (SectionManager::addYear(yearName.trimmed())) {
            yearCombo->clear();
            auto years = manager.getAllYears();
            for (const auto& y : years) yearCombo->addItem(y.name, y.id);
        } else {
            QMessageBox::warning(this, "Error", "Failed to add year. It might already exist.");
        }
    }
}

void OfferingPage::onOpenAssignments() {
    AssignmentDialog dlg(this);
    dlg.exec();
}
