#include "studentpage.h"
#include "studentformdialog.h"
#include "studentdetaildialog.h"
#include "../managers/usermanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QCheckBox>

StudentPage::StudentPage(const QString &role, int id, QWidget *parent) 
    : QWidget(parent), userRole(role), userId(id)
{
    if (userRole == "teacher") {
        // Teacher specific filtering will be implemented via assignments
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(25, 20, 25, 20);

    // ── Page Title ──
    QLabel *title = new QLabel("Students");
    title->setObjectName("pageTitle");
    QLabel *subtitle = new QLabel("Manage student records — add, search, filter, and bulk-edit");
    subtitle->setObjectName("subtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    // ── Top Action Bar ──
    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->setSpacing(10);

    QPushButton *addBtn = new QPushButton("+ Add Student");
    addBtn->setCursor(Qt::PointingHandCursor);
    topBar->addWidget(addBtn);
    
    QPushButton *undoBtn = new QPushButton("↶ Undo Delete");
    undoBtn->setCursor(Qt::PointingHandCursor);
    undoBtn->setObjectName("secondaryButton");
    topBar->addWidget(undoBtn);
    connect(undoBtn, &QPushButton::clicked, this, &StudentPage::onUndoDelete);
    
    if (userRole != "admin") {
        addBtn->setVisible(false);
        undoBtn->setVisible(false);
    }
    
    topBar->addStretch();

    // Sort combo
    QLabel *sortLabel = new QLabel("Sort:");
    sortLabel->setStyleSheet("background:transparent;");
    sortCombo = new QComboBox;
    sortCombo->addItems(QStringList() << "ID (Asc)" << "ID (Desc)"
                                       << "Name (A-Z)" << "Name (Z-A)"
                                       << "Class (A-Z)" << "Class (Z-A)");
    sortCombo->setFixedWidth(140);
    topBar->addWidget(sortLabel);
    topBar->addWidget(sortCombo);
    layout->addLayout(topBar);

    // ── Search & Filter Bar ──
    QGroupBox *searchGroup = new QGroupBox("Search & Filter");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchGroup);
    searchLayout->setSpacing(10);

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Search by name or ID...");
    searchEdit->setClearButtonEnabled(true);
    searchLayout->addWidget(searchEdit, 1);

    searchLayout->addWidget(new QLabel("Class:"));
    classFilterCombo = new QComboBox;
    classFilterCombo->addItem("All");
    classFilterCombo->setFixedWidth(130);
    searchLayout->addWidget(classFilterCombo);

    searchLayout->addWidget(new QLabel("Status:"));
    statusFilterCombo = new QComboBox;
    statusFilterCombo->addItems(QStringList() << "All" << "Active" << "Inactive" << "Graduated");
    statusFilterCombo->setFixedWidth(110);
    searchLayout->addWidget(statusFilterCombo);

    QPushButton *searchBtn = new QPushButton("Search");
    searchBtn->setObjectName("secondaryButton");
    searchBtn->setCursor(Qt::PointingHandCursor);
    searchLayout->addWidget(searchBtn);

    QPushButton *clearBtn = new QPushButton("Clear");
    clearBtn->setObjectName("secondaryButton");
    clearBtn->setCursor(Qt::PointingHandCursor);
    searchLayout->addWidget(clearBtn);

    layout->addWidget(searchGroup);

    // ── Bulk Actions Bar (hidden by default) ──
    bulkBar = new QWidget;
    bulkBar->setObjectName("bulkBar");
    bulkBar->setStyleSheet(
        "#bulkBar { background-color: #1f4068; border-radius: 6px; padding: 6px 12px; }");
    QHBoxLayout *bulkLayout = new QHBoxLayout(bulkBar);
    bulkLayout->setContentsMargins(10, 4, 10, 4);
    bulkLayout->setSpacing(10);

    selectionLabel = new QLabel("0 selected");
    selectionLabel->setStyleSheet("color: #eaeaea; font-weight: bold; background: transparent;");
    bulkLayout->addWidget(selectionLabel);

    QPushButton *bulkDeleteBtn = new QPushButton("Bulk Delete");
    bulkDeleteBtn->setStyleSheet(
        "QPushButton { background-color: #c0392b; padding: 6px 14px; }"
        "QPushButton:hover { background-color: #a93226; }");
    bulkDeleteBtn->setCursor(Qt::PointingHandCursor);
    bulkLayout->addWidget(bulkDeleteBtn);

    bulkLayout->addStretch();

    bulkBar->setVisible(false);
    layout->addWidget(bulkBar);

    // ── Table ──
    table = new QTableWidget;
    table->setColumnCount(8);
    QStringList headers;
    headers << "" << "ID" << "Name" << "Grade" << "Section" << "Stream" << "Status" << "Actions";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->setColumnWidth(0, 35);   // checkbox
    table->setColumnWidth(1, 70);   // ID
    table->setColumnWidth(2, 160);  // Name
    table->setColumnWidth(3, 70);   // Grade
    table->setColumnWidth(4, 70);   // Section
    table->setColumnWidth(5, 120);  // Stream
    table->setColumnWidth(6, 90);   // Status
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);
    layout->addWidget(table, 1);

    // ── Connections ──
    connect(addBtn,    &QPushButton::clicked, this, &StudentPage::onAddStudent);
    connect(searchBtn, &QPushButton::clicked, this, &StudentPage::onSearch);
    connect(searchEdit,&QLineEdit::returnPressed, this, &StudentPage::onSearch);
    connect(clearBtn,  &QPushButton::clicked, this, [this]() {
        searchEdit->clear();
        classFilterCombo->setCurrentIndex(0);
        statusFilterCombo->setCurrentIndex(0);
        refreshTable();
    });
    connect(sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StudentPage::onSortChanged);
    connect(bulkDeleteBtn, &QPushButton::clicked, this, &StudentPage::onBulkDelete);
    connect(table, &QTableWidget::cellDoubleClicked, this, &StudentPage::onRowDoubleClicked);

    refreshTable();
}

// ── Helpers ──

QStringList StudentPage::getUniqueClasses() const {
    QStringList grades;
    auto gradesList = sectionManager.getAllGrades();
    for (const auto& g : gradesList) grades << g.name;
    return grades;
}

QVector<int> StudentPage::getCheckedIds() const {
    QVector<int> ids;
    for (int r = 0; r < table->rowCount(); ++r) {
        QWidget *w = table->cellWidget(r, 0);
        if (!w) continue;
        QCheckBox *cb = w->findChild<QCheckBox*>();
        if (cb && cb->isChecked()) {
            ids.push_back(table->item(r, 1)->text().toInt());
        }
    }
    return ids;
}

// ── Refresh Table ──

void StudentPage::refreshTable() {
    // manager = StudentManager(); // Removed re-initialization that resets sort state

    // Update grade filter combo
    QString prevGrade = classFilterCombo->currentText();
    classFilterCombo->blockSignals(true);
    classFilterCombo->clear();
    classFilterCombo->addItem("All");
    classFilterCombo->addItems(getUniqueClasses());
    int idx = classFilterCombo->findText(prevGrade);
    if (idx >= 0) classFilterCombo->setCurrentIndex(idx);
    classFilterCombo->blockSignals(false);

    // Get filtered data
    QVector<Student> students = manager.filter(
        searchEdit->text().trimmed(),
        classFilterCombo->currentText(),
        statusFilterCombo->currentText(),
        (userRole == "teacher" ? userId : -1)
    );

    table->setRowCount(students.size());
    for (int i = 0; i < students.size(); ++i) {
        const Student &s = students[i];

        // Checkbox
        QWidget *cbContainer = new QWidget;
        QHBoxLayout *cbLayout = new QHBoxLayout(cbContainer);
        cbLayout->setContentsMargins(0, 0, 0, 0);
        cbLayout->setAlignment(Qt::AlignCenter);
        QCheckBox *cb = new QCheckBox;
        connect(cb, &QCheckBox::stateChanged, this, &StudentPage::updateBulkBar);
        cbLayout->addWidget(cb);
        table->setCellWidget(i, 0, cbContainer);

        // Data cells
        table->setItem(i, 1, new QTableWidgetItem(QString::number(s.id)));
        table->setItem(i, 2, new QTableWidgetItem(s.fullName));
        table->setItem(i, 3, new QTableWidgetItem(s.gradeName));
        table->setItem(i, 4, new QTableWidgetItem(s.sectionName));
        table->setItem(i, 5, new QTableWidgetItem(s.streamName.isEmpty() ? "General" : s.streamName));

        // Status with color
        QTableWidgetItem *statusItem = new QTableWidgetItem(s.status);
        if (s.status == "Active")       statusItem->setForeground(QColor("#2ecc71"));
        else if (s.status == "Inactive")  statusItem->setForeground(QColor("#e74c3c"));
        else if (s.status == "Withdrawn") statusItem->setForeground(QColor("#e67e22"));
        table->setItem(i, 6, statusItem);

        // Action buttons
        QWidget *actWidget = new QWidget;
        QHBoxLayout *actLayout = new QHBoxLayout(actWidget);
        actLayout->setContentsMargins(2, 2, 2, 2);
        actLayout->setSpacing(4);

        QPushButton *viewBtn = new QPushButton("View");
        viewBtn->setFixedSize(50, 26);
        viewBtn->setCursor(Qt::PointingHandCursor);
        viewBtn->setStyleSheet(
            "QPushButton { background: #0f3460; font-size: 11px; padding: 2px; border-radius: 4px; min-width: 40px; }"
            "QPushButton:hover { background: #1a5276; }");

        QPushButton *editBtn = new QPushButton("Edit");
        editBtn->setFixedSize(50, 26);
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setStyleSheet(
            "QPushButton { background: #1f4068; font-size: 11px; padding: 2px; border-radius: 4px; min-width: 40px; }"
            "QPushButton:hover { background: #2a5a8c; }");

        QPushButton *delBtn = new QPushButton("Del");
        delBtn->setFixedSize(42, 26);
        delBtn->setCursor(Qt::PointingHandCursor);
        delBtn->setStyleSheet(
            "QPushButton { background: #c0392b; font-size: 11px; padding: 2px; border-radius: 4px; min-width: 35px; }"
            "QPushButton:hover { background: #a93226; }");

        int sid = s.id;
        connect(viewBtn, &QPushButton::clicked, this, [this, sid]() {
            Student st = manager.getStudentById(sid);
            if (st.id == -1) return;
            StudentDetailDialog dlg(st, userRole, this);
            connect(&dlg, &StudentDetailDialog::editRequested, this, &StudentPage::editStudentById);
            dlg.exec();
        });
        connect(editBtn, &QPushButton::clicked, this, [this, sid]() { editStudentById(sid); });
        connect(delBtn,  &QPushButton::clicked, this, [this, sid]() {
            if (QMessageBox::question(this, "Confirm Delete",
                "This will permanently delete student ID " + QString::number(sid) + ".\n\nProceed?",
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                manager.hardDeleteStudent(sid);
                refreshTable();
            }
        });

        actLayout->addWidget(viewBtn);
        if (userRole == "admin") {
            actLayout->addWidget(editBtn);
            actLayout->addWidget(delBtn);
        }
        table->setCellWidget(i, 7, actWidget);

        table->setRowHeight(i, 38);
    }

    updateBulkBar();
}

// ── Add Student ──

void StudentPage::onAddStudent() {
    Student blank;
    blank.id = manager.generateNextId();
    blank.status = "Active";

    StudentFormDialog dlg(blank, false, this);
    if (dlg.exec() == QDialog::Accepted) {
        Student s = dlg.getStudent();
        if (manager.addStudent(s)) {
            refreshTable();
        } else {
            QMessageBox::warning(this, "Error", "Student with this ID already exists.");
        }
    }
}

// ── Edit Student ──

void StudentPage::editStudentById(int id) {
    manager = StudentManager();
    Student s = manager.getStudentById(id);
    if (s.id == -1) return;
    StudentFormDialog dlg(s, true, this);
    if (dlg.exec() == QDialog::Accepted) {
        manager.updateStudent(dlg.getStudent());
        refreshTable();
    }
}

void StudentPage::onEditStudent() {
    int row = table->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Select a student."); return; }
    editStudentById(table->item(row, 1)->text().toInt());
}

// ── View Student ──

void StudentPage::onViewStudent() {
    int row = table->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Select a student."); return; }
    int id = table->item(row, 1)->text().toInt();
    Student s = manager.getStudentById(id);
    if (s.id == -1) return;
    StudentDetailDialog dlg(s, userRole, this);
    connect(&dlg, &StudentDetailDialog::editRequested, this, &StudentPage::editStudentById);
    dlg.exec();
}

// ── Delete Student (safe) ──

void StudentPage::onDeleteStudent() {
    int row = table->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Select a student."); return; }
    int id = table->item(row, 1)->text().toInt();
    if (QMessageBox::question(this, "Confirm Delete",
        "This will permanently delete student ID " + QString::number(id) + " from the database.\n\nProceed?",
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        manager.hardDeleteStudent(id); // Use hard delete, stack will save for Undo
        refreshTable();
    }
}

// ── Bulk Ops ──

void StudentPage::onBulkDelete() {
    QVector<int> ids = getCheckedIds();
    if (ids.isEmpty()) return;
    if (QMessageBox::question(this, "Bulk Delete",
        "Permanently delete " + QString::number(ids.size()) + " student(s) from the database?") == QMessageBox::Yes) {
        manager.bulkHardDelete(ids); // Use hard delete, stack will save for Undo
        refreshTable();
    }
}

void StudentPage::onUndoDelete() {
    if (manager.undoLastDelete()) {
        QMessageBox::information(this, "Undo Successful", "The last deleted student has been restored to 'Active' status.");
        refreshTable();
    } else {
        QMessageBox::warning(this, "Undo Failed", "There are no recent deletions to undo.");
    }
}

void StudentPage::updateBulkBar() {
    QVector<int> ids = getCheckedIds();
    bulkBar->setVisible(!ids.isEmpty());
    selectionLabel->setText(QString::number(ids.size()) + " selected");
}

// ── Search ──

void StudentPage::onSearch() {
    refreshTable(); // refreshTable already reads the filter fields
}

// ── Sort ──

void StudentPage::onSortChanged(int index) {
    // 0=ID Asc, 1=ID Desc, 2=Name A-Z, 3=Name Z-A, 4=Class A-Z, 5=Class Z-A
    StudentManager::SortField field;
    bool asc = true;
    switch (index) {
        case 0: field = StudentManager::ById;    asc = true;  break;
        case 1: field = StudentManager::ById;    asc = false; break;
        case 2: field = StudentManager::ByName;  asc = true;  break;
        case 3: field = StudentManager::ByName;  asc = false; break;
        case 4: field = StudentManager::ByClass; asc = true;  break;
        case 5: field = StudentManager::ByClass; asc = false; break;
        default: field = StudentManager::ById;   asc = true;  break;
    }
    manager.sortBy(field, asc);
    refreshTable();
}

// ── Double-click row → detail view ──

void StudentPage::onRowDoubleClicked(int row, int /*column*/) {
    if (row < 0 || !table->item(row, 1)) return;
    int id = table->item(row, 1)->text().toInt();
    Student s = manager.getStudentById(id);
    if (s.id == -1) return;
    StudentDetailDialog dlg(s, userRole, this);
    connect(&dlg, &StudentDetailDialog::editRequested, this, &StudentPage::editStudentById);
    dlg.exec();
}
