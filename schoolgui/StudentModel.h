#ifndef STUDENT_MODEL_H
#define STUDENT_MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "../modules/students/student.h"

class StudentModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum StudentRoles { NameRole = Qt::UserRole + 1, IdRole };
    
    StudentModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addStudent(QString name, int id);

private:
    StudentManager manager;
    QList<Student> students; // Adapted from Manager
};

#endif
