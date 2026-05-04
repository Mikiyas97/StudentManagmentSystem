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

private slots:
    void onAdd();
    void refreshTable();

private:
    SectionManager manager;
    QTableWidget *table;
    QComboBox *gradeCombo;
    QLineEdit *sectionNameEdit;
    QComboBox *yearCombo;
};

#endif // OFFERINGPAGE_H
