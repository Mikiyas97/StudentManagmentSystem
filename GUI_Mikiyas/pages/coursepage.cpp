#include "coursepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMessageBox>

CoursePage::CoursePage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *title = new QLabel("Manage Subjects");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    // --- Add Subject Section ---
    QWidget *inputArea = new QWidget;
    QHBoxLayout *inputLayout = new QHBoxLayout(inputArea);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Subject Name (e.g. Maths)");
    inputLayout->addWidget(nameEdit, 1);

    gradeCombo = new QComboBox;
    QSqlQuery gq("SELECT id, name FROM grade_levels ORDER BY name ASC");
    while (gq.next()) gradeCombo->addItem(gq.value("name").toString(), gq.value("id").toInt());
    inputLayout->addWidget(new QLabel("Grade:"));
    inputLayout->addWidget(gradeCombo);

    streamCombo = new QComboBox;
    streamCombo->addItem("General", 0);
    QSqlQuery sq("SELECT id, name FROM streams");
    while (sq.next()) streamCombo->addItem(sq.value("name").toString(), sq.value("id").toInt());
    inputLayout->addWidget(new QLabel("Stream:"));
    inputLayout->addWidget(streamCombo);

    QPushButton *addBtn = new QPushButton("Add Subject");
    connect(addBtn, &QPushButton::clicked, this, &CoursePage::onAdd);
    inputLayout->addWidget(addBtn);

    layout->addWidget(inputArea);

    // --- Table ---
    table = new QTableWidget;
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Subject Name", "Grade", "Stream"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    layout->addWidget(table);

    refreshTable();
}

void CoursePage::refreshTable() {
    table->setRowCount(0);
    QSqlQuery query("SELECT s.*, g.name as grade_name, st.name as stream_name "
                   "FROM subjects s "
                   "JOIN grade_levels g ON s.grade_id = g.id "
                   "LEFT JOIN streams st ON s.stream_id = st.id");
    
    while (query.next()) {
        int r = table->rowCount();
        table->insertRow(r);
        table->setItem(r, 0, new QTableWidgetItem(query.value("id").toString()));
        table->setItem(r, 1, new QTableWidgetItem(query.value("name").toString()));
        table->setItem(r, 2, new QTableWidgetItem(query.value("grade_name").toString()));
        QString stream = query.value("stream_name").toString();
        table->setItem(r, 3, new QTableWidgetItem(stream.isEmpty() ? "General" : stream));
    }
}

void CoursePage::onAdd() {
    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) return;

    if (manager.addSubject(name, gradeCombo->currentData().toInt(), streamCombo->currentData().toInt())) {
        nameEdit->clear();
        refreshTable();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add subject.");
    }
}

void CoursePage::onDelete() {
    int row = table->currentRow();
    if (row < 0) return;
    int id = table->item(row, 0)->text().toInt();
    if (manager.deleteSubject(id)) refreshTable();
}
