#ifndef REPORTPAGE_H
#define REPORTPAGE_H

#include <QWidget>
#include <QLabel>

class ReportPage : public QWidget {
    Q_OBJECT
public:
    explicit ReportPage(QWidget *parent = nullptr);

public slots: // Made public for MainWindow to call refreshTable
    void onRefresh();
    void refreshTable() { onRefresh(); }

private:
    QLabel *studentCount;
    QLabel *gradeCount;
    QLabel *courseCount;
    QLabel *teacherCount;
    QLabel *sectionCount;
    QLabel *assignmentCount;
};

#endif // REPORTPAGE_H
