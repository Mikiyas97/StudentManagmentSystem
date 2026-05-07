#include "teacherpage.h"
#include "../managers/subjectmanager.h"
#include "teacherdetaildialog.h"
#include "teacherformdialog.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>


#include <QCheckBox>

TeacherPage::TeacherPage(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(15);
  layout->setContentsMargins(35, 30, 35, 30);

  // ── Page Title ──
  QLabel *title = new QLabel("Teachers Management");
  title->setObjectName("pageTitle");
  QLabel *subtitle = new QLabel(
      "Manage teaching staff — add, search, and assign specializations");
  subtitle->setObjectName("subtitle");
  layout->addWidget(title);
  layout->addWidget(subtitle);

  // ── Top Action Bar ──
  QHBoxLayout *topBar = new QHBoxLayout;
  topBar->setSpacing(10);

  QPushButton *addBtn = new QPushButton("+ Register New Teacher");
  addBtn->setCursor(Qt::PointingHandCursor);
  connect(addBtn, &QPushButton::clicked, this, &TeacherPage::onAddTeacher);
  topBar->addWidget(addBtn);
  topBar->addStretch();

  // Sort combo
  QHBoxLayout *sortLayout = new QHBoxLayout;
  QLabel *sortLabel = new QLabel("Sort:");
  sortLabel->setStyleSheet("background:transparent; color: #a0a0b0;");
  sortCombo = new QComboBox;
  sortCombo->addItem("ID Asc", "id ASC");
  sortCombo->addItem("ID Desc", "id DESC");
  sortCombo->addItem("Name A-Z", "fullName ASC");
  sortCombo->addItem("Name Z-A", "fullName DESC");
  sortCombo->setFixedWidth(140);
  connect(sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &TeacherPage::onSortChanged);
  sortLayout->addWidget(sortLabel);
  sortLayout->addWidget(sortCombo);
  topBar->addLayout(sortLayout);
  layout->addLayout(topBar);

  // ── Search & Filter Bar ──
  QGroupBox *searchGroup = new QGroupBox("Search & Filter");
  QHBoxLayout *searchLayout = new QHBoxLayout(searchGroup);
  searchLayout->setSpacing(10);

  searchEdit = new QLineEdit;
  searchEdit->setPlaceholderText("Search by name, phone or email...");
  searchEdit->setClearButtonEnabled(true);
  searchLayout->addWidget(searchEdit, 1);

  searchLayout->addWidget(new QLabel("Subject:"));
  subjectFilterCombo = new QComboBox;
  subjectFilterCombo->addItem("All");
  subjectFilterCombo->setFixedWidth(150);
  searchLayout->addWidget(subjectFilterCombo);

  QPushButton *searchBtn = new QPushButton("Search");
  searchBtn->setObjectName("secondaryButton");
  searchBtn->setCursor(Qt::PointingHandCursor);
  connect(searchBtn, &QPushButton::clicked, this, &TeacherPage::onSearch);
  searchLayout->addWidget(searchBtn);

  layout->addWidget(searchGroup);

  // ── Bulk Actions Bar ──
  bulkBar = new QWidget;
  bulkBar->setObjectName("bulkBar");
  bulkBar->setStyleSheet("#bulkBar { background-color: #1f4068; border-radius: "
                         "6px; padding: 6px 12px; }");
  QHBoxLayout *bulkLayout = new QHBoxLayout(bulkBar);
  bulkLayout->setContentsMargins(10, 4, 10, 4);
  bulkLayout->setSpacing(10);

  selectionLabel = new QLabel("0 selected");
  selectionLabel->setStyleSheet(
      "color: #eaeaea; font-weight: bold; background: transparent;");
  bulkLayout->addWidget(selectionLabel);

  QPushButton *bulkDeleteBtn = new QPushButton("Bulk Delete");
  bulkDeleteBtn->setStyleSheet(
      "QPushButton { background-color: #c0392b; padding: 6px 14px; } "
      "QPushButton:hover { background-color: #a93226; }");
  bulkDeleteBtn->setCursor(Qt::PointingHandCursor);
  connect(bulkDeleteBtn, &QPushButton::clicked, this,
          &TeacherPage::onBulkDelete);
  bulkLayout->addWidget(bulkDeleteBtn);
  bulkLayout->addStretch();

  bulkBar->setVisible(false);
  layout->addWidget(bulkBar);

  // ── Table ──
  table = new QTableWidget;
  table->setColumnCount(6);
  table->setHorizontalHeaderLabels(
      {"", "ID", "Full Name", "Phone", "Email", "Actions"});

  QHeaderView *header = table->horizontalHeader();

  // Make all columns proportional
  header->setSectionResizeMode(QHeaderView::Stretch);

  // Set proportional sizes
  header->setStretchLastSection(false);
  header->resizeSection(0, 1); // checkbox/icon
  header->resizeSection(1, 2); // ID
  header->resizeSection(2, 5); // Full Name
  header->resizeSection(3, 3); // Phone
  header->resizeSection(4, 5); // Email
  header->resizeSection(5, 3); // Actions

  // Better proportional behavior
  header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  header->setSectionResizeMode(1, QHeaderView::Interactive);
  header->setSectionResizeMode(2, QHeaderView::Stretch);
  header->setSectionResizeMode(3, QHeaderView::Stretch);
  header->setSectionResizeMode(4, QHeaderView::Stretch);
  header->setSectionResizeMode(5, QHeaderView::ResizeToContents);

  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setAlternatingRowColors(true);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  table->verticalHeader()->setVisible(false);
  table->verticalHeader()->setDefaultSectionSize(45);

  table->setStyleSheet(R"(
    QTableWidget::item {
        padding: 5px;
    }
)");

  connect(table, &QTableWidget::cellDoubleClicked, this,
          &TeacherPage::onRowDoubleClicked);

  layout->addWidget(table, 1);

  refreshTable();
}

void TeacherPage::onAddTeacher() {
  TeacherFormDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    Teacher t = dialog.getTeacher();
    t.id = manager.generateNextTeacherId();
    QString pass = "pass" + QString::number(t.id);

    if (manager.addTeacher(t, pass)) {
      QMessageBox::information(
          this, "Success",
          "Teacher Registered!\nID: " + QString::number(t.id) +
              "\nPassword: " + pass);
      refreshTable();
    } else {
      QMessageBox::warning(this, "Error", "Failed to add teacher.");
    }
  }
}

void TeacherPage::onRowDoubleClicked(int row, int /*column*/) {
  if (row < 0 || !table->item(row, 1))
    return;
  onViewTeacher(table->item(row, 1)->text().toInt());
}

void TeacherPage::onSortChanged(int /*index*/) { refreshTable(); }

void TeacherPage::onSearch() { refreshTable(); }

void TeacherPage::onViewTeacher(int id) {
  Teacher t = manager.getTeacherById(id);
  if (t.id != -1) {
    TeacherDetailDialog dialog(t, this);
    dialog.exec();
  }
}

void TeacherPage::onEditTeacher(int /*id*/) {
  QMessageBox::information(this, "Info",
                           "Edit functionality will be implemented soon.");
}

void TeacherPage::onDeleteTeacher(int id) {
  if (QMessageBox::question(this, "Confirm Delete",
                            "Permanently delete teacher ID " +
                                QString::number(id) + "?") ==
      QMessageBox::Yes) {
    if (manager.deleteTeacher(id)) {
      refreshTable();
    } else {
      QMessageBox::warning(this, "Error", "Failed to delete teacher.");
    }
  }
}

void TeacherPage::onBulkDelete() {
  QVector<int> ids = getCheckedIds();
  if (ids.isEmpty())
    return;
  if (QMessageBox::question(this, "Bulk Delete",
                            "Delete " + QString::number(ids.size()) +
                                " teacher(s)?") == QMessageBox::Yes) {
    for (int id : ids)
      manager.deleteTeacher(id);
    refreshTable();
  }
}

void TeacherPage::updateBulkBar() {
  int count = getCheckedIds().size();
  bulkBar->setVisible(count > 0);
  selectionLabel->setText(QString::number(count) + " selected");
}

QVector<int> TeacherPage::getCheckedIds() const {
  QVector<int> ids;
  for (int i = 0; i < table->rowCount(); ++i) {
    QWidget *w = table->cellWidget(i, 0);
    if (w) {
      QCheckBox *cb = w->findChild<QCheckBox *>();
      if (cb && cb->isChecked()) {
        ids.push_back(table->item(i, 1)->text().toInt());
      }
    }
  }
  return ids;
}

void TeacherPage::refreshTable() {
  // Update subject filter combo
  QString prevSub = subjectFilterCombo->currentText();
  subjectFilterCombo->blockSignals(true);
  subjectFilterCombo->clear();
  subjectFilterCombo->addItem("All");
  auto subjects = subjectManager.getAllSubjects();
  for (const auto &sub : subjects)
    subjectFilterCombo->addItem(sub.name);
  int idx = subjectFilterCombo->findText(prevSub);
  if (idx >= 0)
    subjectFilterCombo->setCurrentIndex(idx);
  subjectFilterCombo->blockSignals(false);

  QString sortBy = sortCombo ? sortCombo->currentData().toString() : "";
  QVector<Teacher> teachers =
      manager.filterTeachers(searchEdit->text().trimmed(), sortBy);

  table->setRowCount(teachers.size());
  for (int i = 0; i < teachers.size(); ++i) {
    const Teacher &t = teachers[i];

    // Checkbox
    QWidget *cbContainer = new QWidget;
    cbContainer->setStyleSheet(
        "background: transparent;"); // Allow row highlight to show
    QHBoxLayout *cbLayout = new QHBoxLayout(cbContainer);
    cbLayout->setContentsMargins(0, 0, 0, 0);
    cbLayout->setAlignment(Qt::AlignCenter);
    QCheckBox *cb = new QCheckBox;
    connect(cb, &QCheckBox::stateChanged, this, &TeacherPage::updateBulkBar);
    cbLayout->addWidget(cb);
    table->setCellWidget(i, 0, cbContainer);

    table->setItem(i, 1, new QTableWidgetItem(QString::number(t.id)));
    table->setItem(i, 2, new QTableWidgetItem(t.fullName));
    table->setItem(i, 3, new QTableWidgetItem(t.phone));
    table->setItem(i, 4, new QTableWidgetItem(t.email));

    // Action buttons
    QWidget *actWidget = new QWidget;
    actWidget->setStyleSheet(
        "background: transparent;"); // Allow row highlight to show
    QHBoxLayout *actLayout = new QHBoxLayout(actWidget);
    actLayout->setContentsMargins(8, 2, 8, 2);
    actLayout->setSpacing(8);
    actLayout->setAlignment(Qt::AlignCenter);

    QPushButton *viewBtn = new QPushButton("View");
    viewBtn->setFixedSize(55, 28);
    viewBtn->setCursor(Qt::PointingHandCursor);
    viewBtn->setStyleSheet(
        "QPushButton { background: #0f3460; font-size: 11px; border-radius: "
        "4px; border: 1px solid #1f4068; }"
        "QPushButton:hover { background: #1a5276; border-color: #3498db; }");
    connect(viewBtn, &QPushButton::clicked, this,
            [this, t]() { onViewTeacher(t.id); });

    QPushButton *delBtn = new QPushButton("Del");
    delBtn->setFixedSize(50, 28);
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setStyleSheet(
        "QPushButton { background: #3c1414; font-size: 11px; border-radius: "
        "4px; border: 1px solid #5c1e1e; }"
        "QPushButton:hover { background: #7c2e2e; border-color: #e94560; }");
    connect(delBtn, &QPushButton::clicked, this,
            [this, t]() { onDeleteTeacher(t.id); });

    actLayout->addWidget(viewBtn);
    actLayout->addWidget(delBtn);
    table->setCellWidget(i, 5, actWidget);
  }
  updateBulkBar();
}
