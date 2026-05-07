#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "managers/usermanager.h"
#include "managers/studentmanager.h"

class ProfilePage : public QWidget {
    Q_OBJECT
public:
    explicit ProfilePage(const QString &role, int id, QWidget *parent = nullptr);

public slots:
    void refreshTable();

private:
    void setupUI();
    void clearLayout(QLayout *layout);
    void setupAdminProfile();
    void setupTeacherProfile(int id);
    void setupStudentProfile(int id);
    
    // UI Helpers to match the image
    void createHeader(const QString &name, const QString &id);
    QWidget* createInfoGroup(const QString &title, QMap<QString, QString> details);

    QVBoxLayout *mainLayout;
    QString userRole;
    int userId;
};

#endif // PROFILEPAGE_H