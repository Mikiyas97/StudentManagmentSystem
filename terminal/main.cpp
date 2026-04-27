#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Structure for academic/management data
struct Student {
    int id;
    string firstName;
    string lastName;
    string password;
    string department;
    float gpa;
};

// Node for singly linked list
struct Node {
    Student data;
    Node* next;
    Node(Student s) : data(s), next(NULL) {}
};

class StudentManagementSystem {
private:
    Node* head;

public:
    StudentManagementSystem() : head(NULL) {}

    void addStudent() {
        Student s;
        cout << "\n--- Add New Student ---";
        cout << "\nEnter ID: "; cin >> s.id;
        if (searchStudent(s.id) != NULL) {
            cout << "Error: Student with ID " << s.id << " already exists.\n";
            return;
        }
        cout << "Enter First Name: "; cin >> s.firstName;
        cout << "Enter Last Name: "; cin >> s.lastName;
        cout << "Enter Department: "; cin >> s.department;
        cout << "Enter GPA: "; cin >> s.gpa;
        cout << "Set Login Password: "; cin >> s.password;

        Node* newNode = new Node(s);
        if (head == NULL) head = newNode;
        else {
            Node* temp = head;
            while (temp->next != NULL) temp = temp->next;
            temp->next = newNode;
        }
        saveAllToFile();
        cout << "Student added successfully.\n";
    }

    void deleteStudent(int id) {
        if (head == NULL) return;
        if (head->data.id == id) {
            Node* temp = head;
            head = head->next;
            delete temp;
            saveAllToFile();
            return;
        }
        Node* curr = head;
        while (curr->next != NULL && curr->next->data.id != id) curr = curr->next;
        if (curr->next != NULL) {
            Node* temp = curr->next;
            curr->next = curr->next->next;
            delete temp;
            saveAllToFile();
        }
    }

    void updateStudent(int id) {
        Student* s = searchStudent(id);
        if (s == NULL) return;
        cout << "New First Name: "; cin >> s->firstName;
        cout << "New Last Name: "; cin >> s->lastName;
        cout << "New Dept: "; cin >> s->department;
        cout << "New GPA: "; cin >> s->gpa;
        saveAllToFile();
    }

    Student* searchStudent(int id) {
        Node* temp = head;
        while (temp != NULL) {
            if (temp->data.id == id) return &(temp->data);
            temp = temp->next;
        }
        return NULL;
    }

    void displayStudents() {
        if (head == NULL) { cout << "No records found.\n"; return; }
        cout << "\nID\tName\t\tDept\tGPA\n";
        Node* temp = head;
        while (temp != NULL) {
            cout << temp->data.id << "\t" << temp->data.firstName << " " << temp->data.lastName << "\t" << temp->data.department << "\t" << temp->data.gpa << endl;
            temp = temp->next;
        }
    }

    void saveAllToFile() {
        ofstream outFile("users.txt");
        Node* temp = head;
        while (temp != NULL) {
            outFile << temp->data.id << " " << temp->data.firstName << " " << temp->data.lastName << " " << temp->data.password << " " << temp->data.department << " " << temp->data.gpa << endl;
            temp = temp->next;
        }
        outFile.close();
    }

    void loadFromFile() {
        while(head != NULL) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        ifstream inFile("users.txt");
        if (!inFile) return;
        Student s;
        while (inFile >> s.id >> s.firstName >> s.lastName >> s.password >> s.department >> s.gpa) {
            Node* newNode = new Node(s);
            if (head == NULL) head = newNode;
            else {
                Node* temp = head;
                while (temp->next != NULL) temp = temp->next;
                temp->next = newNode;
            }
        }
        inFile.close();
    }
};

void registerStudent(StudentManagementSystem& sms) {
    Student s;
    cout << "\n--- Student Registration ---\n";
    cout << "Enter Student ID: "; cin >> s.id;
    cout << "Enter First Name: "; cin >> s.firstName;
    cout << "Enter Last Name: "; cin >> s.lastName;
    cout << "Enter Password: "; cin >> s.password;
    s.department = "Unassigned";
    s.gpa = 0.0;

    // Use the class to save to keep format consistent
    // We add it to the list and save
    sms.loadFromFile(); // Ensure we have latest
    if (sms.searchStudent(s.id) == NULL) {
        // We'll manually append to file in the same format for simplicity in this function
        ofstream outFile("users.txt", ios::app);
        outFile << s.id << " " << s.firstName << " " << s.lastName << " " << s.password << " " << s.department << " " << s.gpa << endl;
        outFile.close();
        cout << "Registration successful!\n";
    } else {
        cout << "ID already exists.\n";
    }
}

bool loginStudent(string& loggedInId) {
    string inputId, inputPassword;
    cout << "\n--- Student Login ---\n";
    cout << "Enter ID: "; cin >> inputId;
    cout << "Enter Password: "; cin >> inputPassword;

    ifstream inFile("users.txt");
    string id, fName, lName, pass, dept;
    float gpa;
    bool found = false;

    if (inFile.is_open()) {
        while (inFile >> id >> fName >> lName >> pass >> dept >> gpa) {
            if (id == inputId && pass == inputPassword) {
                found = true;
                loggedInId = id;
                break;
            }
        }
        inFile.close();
    }
    if (found) {
        cout << "Login successful! Welcome, " << fName << ".\n";
        return true;
    } else {
        cout << "Invalid credentials.\n";
        return false;
    }
}

bool loginAdmin() {
    string u, p;
    cout << "Enter Username: "; cin >> u;
    cout << "Enter Password: "; cin >> p;
    return (u == "admin" && p == "admin");
}

int main() {
    StudentManagementSystem sms;
    int choice;
    while (true) {
        sms.loadFromFile(); // Always refresh
        cout << "\n1. Login\n2. Register\n3. Exit\nChoice: ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (choice == 1) {
            int t; cout << "1. Admin\n2. Student\nChoice: "; cin >> t;
            if (t == 1) {
                if (loginAdmin()) {
                    int ac;
                    do {
                        cout << "\n1. Add\n2. Delete\n3. Update\n4. Search\n5. Display\n6. Logout\nChoice: ";
                        cin >> ac;
                        if (ac == 1) sms.addStudent();
                        else if (ac == 2) { int id; cin >> id; sms.deleteStudent(id); }
                        else if (ac == 3) { int id; cin >> id; sms.updateStudent(id); }
                        else if (ac == 4) { int id; cin >> id; Student* s = sms.searchStudent(id); if(s) cout << s->firstName << endl; }
                        else if (ac == 5) sms.displayStudents();
                    } while (ac != 6);
                }
            } else if (t == 2) {
                string sid;
                if (loginStudent(sid)) cout << "Welcome student " << sid << endl;
            }
        } else if (choice == 2) {
            registerStudent(sms);
        } else if (choice == 3) break;
    }
    return 0;
}
