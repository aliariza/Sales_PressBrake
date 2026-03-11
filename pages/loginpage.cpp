#include "loginpage.h"
#include "shortcuthelper.h"
#include "stylehelper.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent),
    titleLabel(nullptr),
    usernameEdit(nullptr),
    passwordEdit(nullptr),
    loginButton(nullptr),
    backButton(nullptr)
{
    setupUi();
}

void LoginPage::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(24, 24, 24, 24);

    auto *container = new QWidget(this);
    container->setMaximumWidth(560);
    container->setObjectName("loginCard");
    container->setStyleSheet(
        "#loginCard {"
        " border: 1px solid #d8d8d8;"
        " border-radius: 10px;"
        " }"
        );

    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(28, 28, 28, 28);
    containerLayout->setSpacing(16);

    titleLabel = new QLabel("Login", container);
    titleLabel->setAlignment(Qt::AlignCenter);

    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    auto *subtitleLabel = new QLabel("Enter your username and password", container);
    subtitleLabel->setAlignment(Qt::AlignCenter);

    QFont subtitleFont;
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);

    auto *userRow = new QHBoxLayout;
    userRow->setSpacing(10);

    auto *userText = new QLabel("Username:", container);
    userText->setMinimumWidth(90);

    usernameEdit = new QLineEdit(container);
    usernameEdit->setMinimumWidth(320);
    usernameEdit->setPlaceholderText("Enter username");

    userRow->addWidget(userText);
    userRow->addWidget(usernameEdit);

    auto *passRow = new QHBoxLayout;
    passRow->setSpacing(10);

    auto *passText = new QLabel("Password:", container);
    passText->setMinimumWidth(90);

    passwordEdit = new QLineEdit(container);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumWidth(320);
    passwordEdit->setPlaceholderText("Enter password");

    passRow->addWidget(passText);
    passRow->addWidget(passwordEdit);

    loginButton = new QPushButton("LOGIN", container);
    backButton = new QPushButton("BACK", container);

    loginButton->setFixedSize(150, 42);
    backButton->setFixedSize(150, 42);

    loginButton->setDefault(true);
    loginButton->setAutoDefault(true);


    StyleHelper::applyPrimaryButtonStyle(loginButton);
    StyleHelper::applySecondaryButtonStyle(backButton);


    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(16);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(backButton);

    containerLayout->addWidget(titleLabel);
    containerLayout->addWidget(subtitleLabel);
    containerLayout->addSpacing(6);
    containerLayout->addLayout(userRow);
    containerLayout->addLayout(passRow);
    containerLayout->addSpacing(8);
    containerLayout->addLayout(buttonLayout);

    rootLayout->addStretch();

    auto *centerLayout = new QHBoxLayout;
    centerLayout->addStretch();
    centerLayout->addWidget(container);
    centerLayout->addStretch();

    rootLayout->addLayout(centerLayout);
    rootLayout->addStretch();

    connect(loginButton, &QPushButton::clicked, this, &LoginPage::loginRequested);
    connect(backButton, &QPushButton::clicked, this, &LoginPage::backRequested);

    connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginPage::loginRequested);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::loginRequested);

    ShortcutHelper::bindEscapeKey(this, this, &LoginPage::backRequested);
}

QString LoginPage::username() const
{
    return usernameEdit->text().trimmed();
}

QString LoginPage::password() const
{
    return passwordEdit->text();
}

void LoginPage::clearInputs()
{
    usernameEdit->clear();
    passwordEdit->clear();
}

void LoginPage::focusUsername()
{
    usernameEdit->setFocus();
}
