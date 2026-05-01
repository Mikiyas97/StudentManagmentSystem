#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>

// This is a minimal, single-file GUI implementation using Qt.
// To run this, you must have Qt installed and configured in your project.

class SMSGui : public QWidget {
public:
    SMSGui(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *label = new QLabel("School Management System", this);
        layout->addWidget(label);
        
        QLineEdit *userEdit = new QLineEdit(this);
        userEdit->setPlaceholderText("Username");
        layout->addWidget(userEdit);
        
        QPushButton *loginBtn = new QPushButton("Login", this);
        layout->addWidget(loginBtn);
        
        connect(loginBtn, &QPushButton::clicked, [=]() {
            QMessageBox::information(this, "Login", "Welcome: " + userEdit->text());
        });
        
        setWindowTitle("School Management System");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SMSGui window;
    window.show();
    return app.exec();
}
