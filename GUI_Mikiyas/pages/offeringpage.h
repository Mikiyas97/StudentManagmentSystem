#ifndef OFFERINGPAGE_H
#define OFFERINGPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

#include "../managers/sectionmanager.h"

class OfferingPage : public QWidget {
    Q_OBJECT
public:
    explicit OfferingPage(QWidget *parent = nullptr);

public slots:
    void refreshTable();

private slots:
    void onAdd();
    void onAddYear();
    void onOpenAssignments();

private:
    SectionManager manager;
    QTableWidget *table;
    QComboBox *gradeCombo;
    QLineEdit *sectionNameEdit;
    QComboBox *yearCombo;
};

#endif // OFFERINGPAGE_H
