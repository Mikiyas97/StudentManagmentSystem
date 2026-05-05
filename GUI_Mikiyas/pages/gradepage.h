#ifndef GRADEPAGE_H
#define GRADEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include "../managers/markmanager.h"

class GradePage : public QWidget {
    Q_OBJECT
public:
    explicit GradePage(const QString &role = "admin", int id = -1,
                       QWidget *parent = nullptr);

private slots:
    void onCalculate();
    void onApprove();
    void refreshTable();

private:
    MarkManager manager;
    QTableWidget *table;
    QComboBox *sectionCombo;
    QComboBox *yearCombo;
    QString userRole;
    int userId;
};

#endif // GRADEPAGE_H
