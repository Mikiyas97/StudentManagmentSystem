#include "studentmanager.h"
#include "usermanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

StudentManager::StudentManager() {
    // Database connection is globally handled in main.cpp
}

// --------------- File I/O is gone ---------------
void StudentManager::loadFromFile() {}
void StudentManager::saveToFile() {}

// --------------- CRUD ---------------

int StudentManager::generateNextId() const {
    QSqlQuery query("SELECT MAX(id) FROM students");
    if (query.next()) {
        return query.value(0).toInt() + 1;
    }
    return 1;
}

bool StudentManager::addStudent(const Student &s) {
    QSqlQuery query;
    query.prepare("INSERT INTO students (id, fullName, gender, date_of_birth, grade_id, section_id, stream_id, phone, email, status) "
                  "VALUES (:id, :name, :gender, :dob, :grade, :section, :stream, :phone, :email, :status)");
    query.bindValue(":id", s.id);
    query.bindValue(":name", s.fullName);
    query.bindValue(":gender", s.gender);
    query.bindValue(":dob", s.dateOfBirth);
    query.bindValue(":grade", s.grade_id);
    query.bindValue(":section", s.section_id);
    query.bindValue(":stream", s.stream_id > 0 ? QVariant(s.stream_id) : QVariant(QVariant::Int));
    query.bindValue(":phone", s.phone);
    query.bindValue(":email", s.email);
    query.bindValue(":status", s.status);
    if (query.exec()) {
        UserManager um;
        User u;
        u.username = QString::number(s.id);
        u.password = "pass" + QString::number(s.id);
        u.role = "student";
        u.relatedId = s.id;
        um.addUser(u);

        // --- Automatic Subject Assignment ---
        // 1. Get current year ID
        int yearId = 1;
        QSqlQuery yq("SELECT id FROM academic_years ORDER BY name DESC LIMIT 1");
        if (yq.next()) yearId = yq.value(0).toInt();

        // 2. Get subjects for this grade and stream
        QSqlQuery sq;
        QString sSql = "SELECT id FROM subjects WHERE grade_id = ? AND (stream_id IS NULL OR stream_id = 0";
        if (s.stream_id > 0) sSql += " OR stream_id = ?)";
        else sSql += ")";
        
        sq.prepare(sSql);
        sq.addBindValue(s.grade_id);
        if (s.stream_id > 0) sq.addBindValue(s.stream_id);

        if (sq.exec()) {
            QSqlQuery mq;
            mq.prepare("INSERT INTO marks (student_id, subject_id, section_id, year_id, score) VALUES (?, ?, ?, ?, 0)");
            while (sq.next()) {
                mq.addBindValue(s.id);
                mq.addBindValue(sq.value(0).toInt());
                mq.addBindValue(s.section_id);
                mq.addBindValue(yearId);
                mq.exec();
            }
        }

        return true;
    }
    return false;
}

bool StudentManager::updateStudent(const Student &s) {
    QSqlQuery query;
    query.prepare("UPDATE students SET fullName=:name, gender=:gender, date_of_birth=:dob, grade_id=:grade, section_id=:section, stream_id=:stream, phone=:phone, "
                  "email=:email, status=:status WHERE id=:id");
    query.bindValue(":id", s.id);
    query.bindValue(":name", s.fullName);
    query.bindValue(":gender", s.gender);
    query.bindValue(":dob", s.dateOfBirth);
    query.bindValue(":grade", s.grade_id);
    query.bindValue(":section", s.section_id);
    query.bindValue(":stream", s.stream_id > 0 ? QVariant(s.stream_id) : QVariant(QVariant::Int));
    query.bindValue(":phone", s.phone);
    query.bindValue(":email", s.email);
    query.bindValue(":status", s.status);
    return query.exec();
}

bool StudentManager::softDeleteStudent(int id) {
    QSqlQuery query;
    query.prepare("UPDATE students SET status='Inactive' WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

bool StudentManager::hardDeleteStudent(int id) {
    // 1. Fetch student before deletion
    Student s = getStudentById(id);
    if (s.id == -1) return false;
    
    // 2. Push to stack for undo feature
    QVector<Student> batch; batch.push_back(s);
    undoStack.push(batch);
    
    // 3. Delete from DB
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

bool StudentManager::undoLastDelete() {
    if (undoStack.isEmpty()) return false;
    
    QVector<Student> batch = undoStack.pop(); // Pop from LIFO Stack
    bool success = true;
    
    for (const Student& s : batch) {
        QSqlQuery query;
        query.prepare("INSERT INTO students (id, fullName, gender, date_of_birth, grade_id, section_id, stream_id, phone, email, status) "
                      "VALUES (:id, :name, :gender, :dob, :grade, :section, :stream, :phone, :email, :status)");
        query.bindValue(":id", s.id);
        query.bindValue(":name", s.fullName);
        query.bindValue(":gender", s.gender);
        query.bindValue(":dob", s.dateOfBirth);
        query.bindValue(":grade", s.grade_id);
        query.bindValue(":section", s.section_id);
        query.bindValue(":stream", s.stream_id > 0 ? QVariant(s.stream_id) : QVariant(QVariant::Int));
        query.bindValue(":phone", s.phone);
        query.bindValue(":email", s.email);
        query.bindValue(":status", s.status);
        if (!query.exec()) success = false;
    }
    
    return success;
}

void StudentManager::bulkSoftDelete(const QVector<int> &ids) {
    QSqlQuery query;
    query.prepare("UPDATE students SET status='Inactive' WHERE id=:id");
    for (int id : ids) {
        query.bindValue(":id", id);
        query.exec();
    }
}

void StudentManager::bulkHardDelete(const QVector<int> &ids) {
    // 1. Fetch students before deletion
    QVector<Student> batch;
    for (int id : ids) {
        Student s = getStudentById(id);
        if (s.id != -1) batch.push_back(s);
    }
    
    // 2. Push to stack for undo feature
    if (!batch.isEmpty()) {
        undoStack.push(batch);
    }

    // 3. Delete from DB
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE id=:id");
    for (int id : ids) {
        query.bindValue(":id", id);
        query.exec();
    }
}

void StudentManager::bulkAssignClass(const QVector<int> &ids, const QString &sectionIdStr) {
    // 1. Enqueue all IDs into our custom FIFO Queue
    Queue<int> processingQueue;
    for (int id : ids) {
        processingQueue.enqueue(id);
    }
    
    int secId = sectionIdStr.toInt();
    QSqlQuery query;
    query.prepare("UPDATE students SET section_id=:sec WHERE id=:id");
    
    // 2. Process batch operation by dequeuing one by one (FIFO order)
    while (!processingQueue.isEmpty()) {
        int currentId = processingQueue.dequeue();
        
        query.bindValue(":id", currentId);
        query.bindValue(":sec", secId);
        query.exec();
    }
}

// --------------- Query ---------------

static Student parseStudent(QSqlQuery &q) {
    Student s;
    s.id = q.value("id").toInt();
    s.fullName = q.value("fullName").toString();
    s.gender = q.value("gender").toString();
    s.dateOfBirth = q.value("date_of_birth").toString();
    s.grade_id = q.value("grade_id").toInt();
    s.gradeName = q.value("grade_name").toString();
    s.section_id = q.value("section_id").toInt();
    s.sectionName = q.value("section_name").toString();
    s.stream_id = q.value("stream_id").toInt();
    s.streamName = q.value("stream_name").toString();
    s.phone = q.value("phone").toString();
    s.email = q.value("email").toString();
    s.status = q.value("status").toString();
    return s;
}

LinkedList<Student> StudentManager::loadStudentsAsLinkedList(int teacherId) const {
    LinkedList<Student> list;
    
    QString sql = "SELECT s.*, g.name as grade_name, sec.name as section_name, st.name as stream_name "
                  "FROM students s "
                  "LEFT JOIN grade_levels g ON s.grade_id = g.id "
                  "LEFT JOIN sections sec ON s.section_id = sec.id "
                  "LEFT JOIN streams st ON s.stream_id = st.id";
                  
    if (teacherId > 0) {
        sql += " WHERE s.section_id IN (SELECT section_id FROM teaching_assignments WHERE teacher_id = ?)";
    }
    
    QSqlQuery query;
    query.prepare(sql);
    if (teacherId > 0) query.addBindValue(teacherId);
    
    if (query.exec()) {
        while (query.next()) {
            list.push_back(parseStudent(query));
        }
    }
    return list;
}

QVector<Student> StudentManager::getStudents() const {
    // 1. Load all students into a linked list
    LinkedList<Student> list = loadStudentsAsLinkedList();
    
    // 2. Convert to QVector for UI consumption
    return list.toQVector();
}

// ========================================================================
// Algorithm: Binary Search — O(log n)
// ========================================================================
// This function demonstrates a custom binary search algorithm without using
// the standard library. Binary search requires random access (O(1) indexing),
// which is why we convert the linked list to a QVector first. 
// Time Complexity: O(log n) where n is the number of students.
// ========================================================================
Student StudentManager::getStudentById(int id) const {
    // 1. Load all students into a LinkedList
    LinkedList<Student> list = loadStudentsAsLinkedList();
    
    // 2. Ensure they are sorted by ID using our O(n log n) merge sort
    list.mergeSort([](const Student& a, const Student& b) {
        return a.id < b.id; // Ascending order
    });
    
    // 3. Convert to QVector for O(1) random access
    QVector<Student> sortedVector = list.toQVector();
    
    // 4. Custom Binary Search implementation (no std::binary_search)
    int left = 0;
    int right = sortedVector.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (sortedVector[mid].id == id) {
            return sortedVector[mid]; // Match found
        }
        
        if (sortedVector[mid].id < id) {
            left = mid + 1; // Search right half
        } else {
            right = mid - 1; // Search left half
        }
    }
    
    // Return empty student if not found
    Student empty; empty.id = -1; return empty;
}

QVector<Student> StudentManager::filter(const QString &nameOrId,
                                         const QString &gradeFilter,
                                         const QString &statusFilter,
                                         int teacherId) const {
    // 1. Load data from SQLite into a Linked List (teacher scope already applied)
    LinkedList<Student> allStudents = loadStudentsAsLinkedList(teacherId);
    
    // 2. Filter the data in-memory using O(n) linked list traversal
    LinkedList<Student> filtered = allStudents.findAll([&](const Student& s) {
        // Name/ID match
        bool matchesName = true;
        if (!nameOrId.isEmpty()) {
            QString lowerSearch = nameOrId.toLower();
            matchesName = s.fullName.toLower().contains(lowerSearch) || 
                          QString::number(s.id).contains(lowerSearch);
        }
        
        // Grade match
        bool matchesGrade = true;
        if (!gradeFilter.isEmpty() && gradeFilter != "All") {
            matchesGrade = (s.gradeName == gradeFilter);
        }
        
        // Status match
        bool matchesStatus = true;
        if (!statusFilter.isEmpty() && statusFilter != "All") {
            matchesStatus = (s.status == statusFilter);
        }
        
        return matchesName && matchesGrade && matchesStatus;
    });
    
    // 3. Sort the filtered data using O(n log n) merge sort on the linked list
    bool asc = currentSortAscending;
    SortField field = currentSortField;
    
    filtered.mergeSort([asc, field](const Student& a, const Student& b) {
        if (field == ByName) {
            return asc ? (a.fullName.toLower() < b.fullName.toLower()) 
                       : (a.fullName.toLower() > b.fullName.toLower());
        } else if (field == ByClass) {
            // Sort by section name
            return asc ? (a.sectionName.toLower() < b.sectionName.toLower())
                       : (a.sectionName.toLower() > b.sectionName.toLower());
        } else {
            // Default: Sort by ID
            return asc ? (a.id < b.id) : (a.id > b.id);
        }
    });
    
    // 4. Convert the final processed linked list to a QVector for the UI
    return filtered.toQVector();
}

// --------------- Sorting ---------------

void StudentManager::sortBy(SortField field, bool ascending) {
    currentSortField = field;
    currentSortAscending = ascending;
}
