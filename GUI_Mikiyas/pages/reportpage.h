#ifndef REPORTPAGE_H
#define REPORTPAGE_H

#include <QWidget>
#include <QLabel>

class ReportPage : public QWidget {
    Q_OBJECT
public:
    explicit ReportPage(QWidget *parent = nullptr);

private slots:
    void onRefresh();

private:
    QLabel *studentCount;
    QLabel *gradeCount;
    QLabel *courseCount;
};

#endif // REPORTPAGE_H
