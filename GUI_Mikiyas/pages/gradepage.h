#ifndef GRADEPAGE_H
#define GRADEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include "../managers/markmanager.h"
#include "../managers/sectionmanager.h"

class GradePage : public QWidget {
    Q_OBJECT
public:
    explicit GradePage(const QString &role = "admin", int id = -1,
                       QWidget *parent = nullptr);

public slots:
    void refreshTable();

private slots:
    void onCalculate();
    void onApprove();

private:
    MarkManager manager;
    SectionManager sectionManager;
    QTableWidget *table;
    QComboBox *gradeCombo;
    QComboBox *streamCombo;
    QComboBox *sectionCombo;
    QComboBox *yearCombo;
    QComboBox *semesterCombo;
    QString userRole;
    int userId;
};

#endif // GRADEPAGE_H
