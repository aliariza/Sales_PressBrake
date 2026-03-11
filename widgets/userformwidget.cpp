#include "userformwidget.h"
#include "stylehelper.h"

#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>

UserFormWidget::UserFormWidget(QWidget *parent)
    : QWidget(parent),
    m_userNoEdit(nullptr),
    m_usernameEdit(nullptr),
    m_passwordEdit(nullptr),
    m_roleComboBox(nullptr),
    m_commentsEdit(nullptr),
    m_submitButton(nullptr),
    m_cancelEditButton(nullptr),
    m_statusLabel(nullptr)
{
    setupUi();
    setCreateMode();
}

void UserFormWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Create / Update User", this);
    group->setFont(sectionFont);

    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setContentsMargins(16, 20, 16, 16);
    groupLayout->setSpacing(12);

    auto *topRow = new QHBoxLayout;
    topRow->setSpacing(16);

    auto *noPair = new QHBoxLayout;
    noPair->setSpacing(6);
    auto *noText = new QLabel("No:", this);
    m_userNoEdit = new QLineEdit(this);
    m_userNoEdit->setReadOnly(true);
    m_userNoEdit->setFixedWidth(80);
    m_userNoEdit->setToolTip("Auto-generated user number. Read-only.");
    noPair->addWidget(noText);
    noPair->addWidget(m_userNoEdit);

    auto *userPair = new QHBoxLayout;
    userPair->setSpacing(6);
    auto *userText = new QLabel("User:", this);
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setMinimumWidth(220);
    m_usernameEdit->setPlaceholderText("Enter username");
    m_usernameEdit->setToolTip("Username.");
    m_usernameEdit->setClearButtonEnabled(true);
    userPair->addWidget(userText);
    userPair->addWidget(m_usernameEdit);

    auto *rolePair = new QHBoxLayout;
    rolePair->setSpacing(6);
    auto *roleText = new QLabel("Role:", this);
    m_roleComboBox = new QComboBox(this);
    m_roleComboBox->addItem("user");
    m_roleComboBox->addItem("admin");
    m_roleComboBox->setFixedWidth(120);
    m_roleComboBox->setToolTip("User role.");
    rolePair->addWidget(roleText);
    rolePair->addWidget(m_roleComboBox);

    topRow->addLayout(noPair);
    topRow->addLayout(userPair, 1);
    topRow->addLayout(rolePair);

    auto *bottomRow = new QHBoxLayout;
    bottomRow->setSpacing(16);

    auto *passPair = new QHBoxLayout;
    passPair->setSpacing(6);
    auto *passText = new QLabel("Password:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumWidth(180);
    m_passwordEdit->setPlaceholderText("Enter password");
    m_passwordEdit->setToolTip("Password for new user. Disabled during edit mode.");
    passPair->addWidget(passText);
    passPair->addWidget(m_passwordEdit);

    auto *commentsPair = new QHBoxLayout;
    commentsPair->setSpacing(6);
    auto *commentsText = new QLabel("Comments:", this);
    m_commentsEdit = new QLineEdit(this);
    m_commentsEdit->setMinimumWidth(280);
    m_commentsEdit->setPlaceholderText("Optional notes");
    m_commentsEdit->setToolTip("Notes or comments for this user.");
    m_commentsEdit->setClearButtonEnabled(true);
    commentsPair->addWidget(commentsText);
    commentsPair->addWidget(m_commentsEdit, 1);

    m_submitButton = new QPushButton("CREATE USER", this);
    m_submitButton->setFixedHeight(34);
    m_submitButton->setMinimumWidth(130);
    StyleHelper::applyPrimaryButtonStyle(m_submitButton);

    m_cancelEditButton = new QPushButton("CANCEL EDIT", this);
    m_cancelEditButton->setFixedHeight(34);
    m_cancelEditButton->setMinimumWidth(130);
    StyleHelper::applySecondaryButtonStyle(m_cancelEditButton);
    m_cancelEditButton->hide();

    auto *buttonRow = new QHBoxLayout;
    buttonRow->setSpacing(8);
    buttonRow->addWidget(m_submitButton);
    buttonRow->addWidget(m_cancelEditButton);

    bottomRow->addLayout(passPair);
    bottomRow->addLayout(commentsPair, 1);
    bottomRow->addLayout(buttonRow);

    m_statusLabel = new QLabel("", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    groupLayout->addLayout(topRow);
    groupLayout->addLayout(bottomRow);
    groupLayout->addSpacing(4);
    groupLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(group);

    connect(m_submitButton, &QPushButton::clicked, this, &UserFormWidget::submitRequested);
    connect(m_cancelEditButton, &QPushButton::clicked, this, &UserFormWidget::cancelEditRequested);

    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &UserFormWidget::submitRequested);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &UserFormWidget::submitRequested);
    connect(m_commentsEdit, &QLineEdit::returnPressed, this, &UserFormWidget::submitRequested);
}

QString UserFormWidget::userNo() const
{
    return m_userNoEdit->text().trimmed();
}

QString UserFormWidget::username() const
{
    return m_usernameEdit->text().trimmed();
}

QString UserFormWidget::password() const
{
    return m_passwordEdit->text();
}

QString UserFormWidget::role() const
{
    return m_roleComboBox->currentText();
}

QString UserFormWidget::comments() const
{
    return m_commentsEdit->text().trimmed();
}

void UserFormWidget::setCreateMode()
{
    clearForm();
    m_passwordEdit->setEnabled(true);
    m_passwordEdit->setPlaceholderText("Enter password");
    m_submitButton->setText("CREATE USER");
    m_cancelEditButton->hide();
    setStyleSheet("");
}

void UserFormWidget::setEditMode(const User &user)
{
    m_userNoEdit->setText(user.userno);
    m_usernameEdit->setText(user.username);
    m_passwordEdit->clear();
    m_passwordEdit->setPlaceholderText("Password unchanged");
    m_passwordEdit->setEnabled(false);
    m_commentsEdit->setText(user.comments);

    const int roleIndex = m_roleComboBox->findText(user.role);
    if (roleIndex >= 0)
        m_roleComboBox->setCurrentIndex(roleIndex);

    m_submitButton->setText("UPDATE USER");
    m_cancelEditButton->show();

    setStyleSheet(
        "QGroupBox {"
        " border: 1px solid #7aa7ff;"
        " border-radius: 6px;"
        " margin-top: 8px;"
        " }"
        "QGroupBox::title {"
        " subcontrol-origin: margin;"
        " left: 10px;"
        " padding: 0 4px;"
        " }"
        );
}

void UserFormWidget::clearForm()
{
    m_userNoEdit->clear();
    m_usernameEdit->clear();
    m_passwordEdit->clear();
    m_commentsEdit->clear();
    m_roleComboBox->setCurrentIndex(0);
}

void UserFormWidget::setStatusMessage(const QString &text)
{
    m_statusLabel->setText(text);

    if (text.startsWith("Error:", Qt::CaseInsensitive)) {
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600; padding-top: 4px;");
    } else if (text.contains("successfully", Qt::CaseInsensitive)) {
        m_statusLabel->setStyleSheet("color: #2E7D32; font-weight: 600; padding-top: 4px;");
    } else {
        m_statusLabel->setStyleSheet("color: #444444; font-weight: 500; padding-top: 4px;");
    }
}

void UserFormWidget::clearStatusMessage()
{
    m_statusLabel->clear();
    m_statusLabel->setStyleSheet("");
}

void UserFormWidget::focusUsername()
{
    m_usernameEdit->setFocus();
}
