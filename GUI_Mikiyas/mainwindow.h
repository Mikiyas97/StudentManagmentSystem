#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVector>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(const QString &role, int studentId, QWidget *parent = nullptr);

private slots:
    void switchPage(int index);
    void onGlobalRefresh();

private:
    void setupUI();
    QPushButton* makeNavButton(const QString &text, int index);

    QString userRole;
    int userStudentId;
    QStackedWidget *stack;
    QVector<QPushButton*> navButtons;
    int activePage;
    QLabel *roleLabel;
};

#endif // MAINWINDOW_H
