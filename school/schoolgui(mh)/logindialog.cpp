#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), studentId(-1)
{
    setWindowTitle("School Management System");
    setFixedSize(420, 480);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(40, 40, 40, 30);

    // Icon / Title
    QLabel *iconLabel = new QLabel(QString::fromUtf8("\xF0\x9F\x8E\x93")); // graduation cap emoji
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 48px; background: transparent; margin-bottom: 5px;");

    QLabel *titleLabel = new QLabel("School Management\nSystem");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: #e94560;"
        "background: transparent; margin-bottom: 25px;"
    );

    // Card container
    QWidget *card = new QWidget;
    card->setObjectName("loginCard");
    card->setStyleSheet(
        "#loginCard { background-color: #16213e; border-radius: 12px; padding: 25px; }"
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(8);

    QLabel *usernameLabel = new QLabel("Username");
    usernameLabel->setStyleSheet("font-size: 13px; color: #a0a0b0; font-weight: bold;");
    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Enter username or student ID");

    QLabel *passwordLabel = new QLabel("Password");
    passwordLabel->setStyleSheet("font-size: 13px; color: #a0a0b0; font-weight: bold;");
    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Enter password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *loginBtn = new QPushButton("Login");
    loginBtn->setCursor(Qt::PointingHandCursor);
    loginBtn->setStyleSheet(
        "QPushButton { background-color: #e94560; color: white; border: none;"
        "border-radius: 8px; padding: 12px; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background-color: #c81e45; }"
        "QPushButton:pressed { background-color: #a01535; }"
    );

    errorLabel = new QLabel("");
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setStyleSheet("color: #ff6b6b; font-size: 12px; background: transparent;");

    cardLayout->addWidget(usernameLabel);
    cardLayout->addWidget(usernameEdit);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(passwordLabel);
    cardLayout->addWidget(passwordEdit);
    cardLayout->addSpacing(12);
    cardLayout->addWidget(loginBtn);
    cardLayout->addWidget(errorLabel);

    // Hint
    QLabel *hintLabel = new QLabel("Admin: admin / admin123\nStudent: <ID> / pass<ID>");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet("color: #444; font-size: 11px; margin-top: 15px; background: transparent;");

    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(card);
    mainLayout->addWidget(hintLabel);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
}

void LoginDialog::onLogin() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    role = "";
    studentId = -1;

    if (username.isEmpty() || password.isEmpty()) {
        errorLabel->setText("Please enter both fields.");
        return;
    }

    // Admin check
    if (username == "admin" && password == "admin123") {
        role = "admin";
        accept();
        return;
    }

    // Student check: username is numeric ID, password is "pass" + ID
    bool isDigits = true;
    for (int i = 0; i < username.length(); ++i) {
        if (!username[i].isDigit()) { isDigits = false; break; }
    }

    if (isDigits && !username.isEmpty()) {
        QString expectedPass = "pass" + username;
        if (password == expectedPass) {
            role = "student";
            studentId = username.toInt();
            accept();
            return;
        }
    }

    errorLabel->setText("Invalid credentials!");
}

QString LoginDialog::getRole() const { return role; }
int LoginDialog::getStudentId() const { return studentId; }
