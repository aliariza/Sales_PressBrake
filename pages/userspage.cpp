#include "userspage.h"
#include "shortcuthelper.h"
#include "userstablewidget.h"
#include "userformwidget.h"
#include "stylehelper.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

UsersPage::UsersPage(QWidget *parent)
    : QWidget(parent),
      adminLogoutButton(nullptr),
      m_openMaterialsButton(nullptr),
      m_userFormWidget(nullptr),
      m_usersTableWidget(nullptr),
      m_isEditMode(false)
{
    QString errorMessage;
    if (!m_userRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    setupUi();
    refreshUserTable();
}

void UsersPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 20);
    layout->setSpacing(16);

    auto *topBarLayout = new QHBoxLayout;


    m_userFormWidget = new UserFormWidget(this);
    m_usersTableWidget = new UsersTableWidget(this);

    layout->addLayout(topBarLayout);
    layout->addWidget(m_userFormWidget);
    layout->addWidget(m_usersTableWidget, 1);


    connect(m_userFormWidget, &UserFormWidget::submitRequested,
            this, &UsersPage::createOrUpdateUser);
    connect(m_userFormWidget, &UserFormWidget::cancelEditRequested,
            this, &UsersPage::cancelEdit);

    connect(m_usersTableWidget, &UsersTableWidget::editRequested,
            this, &UsersPage::handleEditRequested);
    connect(m_usersTableWidget, &UsersTableWidget::deleteRequested,
            this, &UsersPage::handleDeleteRequested);


}

void UsersPage::enterEditMode(const User &user)
{
    m_isEditMode = true;
    m_editingUserNo = user.userno;

    m_userFormWidget->setEditMode(user);
    m_userFormWidget->setStatusMessage("Edit mode.");
    m_userFormWidget->focusUsername();
}

void UsersPage::exitEditMode()
{
    m_isEditMode = false;
    m_editingUserNo.clear();

    m_userFormWidget->setCreateMode();
}

void UsersPage::cancelEdit()
{
    exitEditMode();
    m_userFormWidget->clearStatusMessage();
    m_userFormWidget->focusUsername();
}

void UsersPage::refreshUserTable()
{
    QString errorMessage;
    const QList<User> users = m_userRepository.getAllUsers(errorMessage);

    if (!errorMessage.isEmpty()) {
        m_userFormWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_usersTableWidget->setUsers(users);
}

void UsersPage::handleEditRequested(const User &user)
{
    m_userFormWidget->clearStatusMessage();
    enterEditMode(user);
}

void UsersPage::handleDeleteRequested(const User &user)
{
    const QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Delete User",
                              "Delete user '" + user.username + "'?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!m_userRepository.deleteUser(user.username, errorMessage)) {
        m_userFormWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_userFormWidget->setStatusMessage("User deleted successfully.");

    if (m_isEditMode && m_editingUserNo == user.userno)
        exitEditMode();

    refreshUserTable();
}

void UsersPage::createOrUpdateUser()
{
    const QString username = m_userFormWidget->username();
    const QString password = m_userFormWidget->password();
    const QString role = m_userFormWidget->role();
    const QString comments = m_userFormWidget->comments();

    QString errorMessage;

    if (m_isEditMode) {
        User updatedUser;
        updatedUser.userno = m_editingUserNo;
        updatedUser.username = username;
        updatedUser.role = role;
        updatedUser.comments = comments;

        if (!m_userRepository.updateUser(m_editingUserNo, updatedUser, errorMessage)) {
            m_userFormWidget->setStatusMessage("Error: " + errorMessage);
            return;
        }

        exitEditMode();
        m_userFormWidget->setStatusMessage("User updated successfully.");
        refreshUserTable();
        m_userFormWidget->focusUsername();
        return;
    }

    const QString userNo = m_userRepository.getNextUserNo(errorMessage);

    if (!errorMessage.isEmpty()) {
        m_userFormWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    User newUser;
    newUser.userno = userNo;
    newUser.username = username;
    newUser.password = password;
    newUser.role = role;
    newUser.comments = comments;

    if (!m_userRepository.addUser(newUser, errorMessage)) {
        m_userFormWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_userFormWidget->setCreateMode();
    m_userFormWidget->setStatusMessage("User created successfully.");
    m_userFormWidget->focusUsername();

    refreshUserTable();
}
void UsersPage::requestLogout()
{
    if (m_isEditMode) {
        const QMessageBox::StandardButton reply =
            QMessageBox::question(this,
                                  "Discard Edit",
                                  "You are editing a user. Discard changes and logout?",
                                  QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes)
            return;
    }

    emit logoutRequested();
}

