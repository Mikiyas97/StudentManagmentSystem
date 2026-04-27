#include <QApplication>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <fstream>
#include <iostream>

using namespace std;

// ================= BACKEND =================
struct Student {
    int id;
    string firstName;
    string lastName;
    string password;
    string department;
    float gpa;
};

struct Node {
    Student data;
    Node* next;
    Node(Student s) : data(s), next(NULL) {}
};

class SMS {
private:
    Node* head;

public:
    SMS() : head(NULL) {}

    Node* getHead() { return head; }

    void addStudent(Student s) {
        Node* newNode = new Node(s);
        if (!head) head = newNode;
        else {
            Node* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
        save();
    }

    void save() {
        ofstream f("users.txt");
        Node* t = head;
        while (t) {
            f << t->data.id << " " << t->data.firstName << " "
              << t->data.lastName << " " << t->data.password << " "
              << t->data.department << " " << t->data.gpa << endl;
            t = t->next;
        }
    }

    void load() {
        head = NULL;
        ifstream f("users.txt");
        Student s;
        while (f >> s.id >> s.firstName >> s.lastName >> s.password >> s.department >> s.gpa) {
            addStudent(s);
        }
    }
};

// ================= GUI =================
class MainWindow : public QMainWindow {
public:
    SMS sms;
    QStackedWidget *stack;

    // login
    QLineEdit *idInput, *passInput;
    QComboBox *roleBox;

    // admin
    QTableWidget *table;
    QLineEdit *idField, *fname, *lname, *dept, *gpa;

    MainWindow() {
        setWindowTitle("Student Management System");
        resize(900, 600);

        stack = new QStackedWidget();

        // ---------- LOGIN/HOME PAGE ----------
        QWidget *loginPage = new QWidget();
        QVBoxLayout *loginLayout = new QVBoxLayout();

        QLabel *title = new QLabel("🎓 Student Management System");
        title->setStyleSheet("font-size:24px; font-weight:bold; color:white;");
        title->setAlignment(Qt::AlignCenter);

        roleBox = new QComboBox();
        roleBox->addItems({"Student", "Admin"});

        idInput = new QLineEdit(); idInput->setPlaceholderText("ID/Username");
        passInput = new QLineEdit(); passInput->setPlaceholderText("Password");
        passInput->setEchoMode(QLineEdit::Password);

        QPushButton *loginBtn = new QPushButton("Login");
        QPushButton *regNavBtn = new QPushButton("Register");
        regNavBtn->setStyleSheet("background:#334155;");

        loginLayout->addWidget(title);
        loginLayout->addWidget(roleBox);
        loginLayout->addWidget(idInput);
        loginLayout->addWidget(passInput);
        loginLayout->addWidget(loginBtn);
        loginLayout->addWidget(regNavBtn);
        loginPage->setLayout(loginLayout);

        // ---------- ADMIN PAGE ----------
        QWidget *adminPage = new QWidget();
        QVBoxLayout *adminLayout = new QVBoxLayout();

        table = new QTableWidget();
        table->setColumnCount(4);
        table->setHorizontalHeaderLabels({"ID","Name","Dept","GPA"});
        table->horizontalHeader()->setStretchLastSection(true);

        idField = new QLineEdit(); idField->setPlaceholderText("ID");
        fname = new QLineEdit(); fname->setPlaceholderText("First Name");
        lname = new QLineEdit(); lname->setPlaceholderText("Last Name");
        dept = new QLineEdit(); dept->setPlaceholderText("Department");
        gpa = new QLineEdit(); gpa->setPlaceholderText("GPA");

        QPushButton *addBtn = new QPushButton("Add Student");

        adminLayout->addWidget(table);
        adminLayout->addWidget(idField);
        adminLayout->addWidget(fname);
        adminLayout->addWidget(lname);
        adminLayout->addWidget(dept);
        adminLayout->addWidget(gpa);
        adminLayout->addWidget(addBtn);

        adminPage->setLayout(adminLayout);

        // ---------- STACK ----------
        stack->addWidget(loginPage);
        stack->addWidget(adminPage);

        setCentralWidget(stack);

        // ---------- STYLE ----------
        setStyleSheet(R"(
            QMainWindow { background:#0f172a; }
            QLineEdit {
                background:#1e293b; color:white;
                border:2px solid #334155;
                border-radius:8px; padding:6px;
            }
            QPushButton {
                background:#2563eb; color:white;
                border-radius:10px; padding:8px;
                font-weight:bold;
            }
            QPushButton:hover { background:#1d4ed8; }
            QTableWidget {
                background:#1e293b; color:white;
                gridline-color:#334155;
            }
        )");

        // ---------- LOGIC ----------
        sms.load();
        loadTable();

        QObject::connect(loginBtn, &QPushButton::clicked, [&]() {
            if (roleBox->currentText() == "Admin") {
                if (idInput->text() == "admin" && passInput->text() == "admin") {
                    stack->setCurrentIndex(1);
                } else {
                    QMessageBox::warning(this,"Error","Wrong credentials");
                }
            }
        });

        QObject::connect(addBtn, &QPushButton::clicked, [&]() {
            Student s;
            s.id = idField->text().toInt();
            s.firstName = fname->text().toStdString();
            s.lastName = lname->text().toStdString();
            s.department = dept->text().toStdString();
            s.gpa = gpa->text().toFloat();
            s.password = "1234";

            sms.addStudent(s);
            loadTable();
        });
    }

    void loadTable() {
        table->setRowCount(0);
        Node* t = sms.getHead();
        int row = 0;

        while (t) {
            table->insertRow(row);
            table->setItem(row,0,new QTableWidgetItem(QString::number(t->data.id)));
            table->setItem(row,1,new QTableWidgetItem(QString::fromStdString(t->data.firstName)));
            table->setItem(row,2,new QTableWidgetItem(QString::fromStdString(t->data.department)));
            table->setItem(row,3,new QTableWidgetItem(QString::number(t->data.gpa)));
            t = t->next;
            row++;
        }
    }
};

// ================= MAIN =================
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}