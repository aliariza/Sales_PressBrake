#ifndef USERSPAGE_H
#define USERSPAGE_H

#include <QWidget>
#include "userrepository.h"

class QPushButton;
class UsersTableWidget;
class UserFormWidget;

class UsersPage : public QWidget
{
    Q_OBJECT

public:
    explicit UsersPage(QWidget *parent = nullptr);

    void refreshUserTable();

signals:
    void logoutRequested();
    void materialsRequested();

private slots:
    void createOrUpdateUser();
    void cancelEdit();
    void handleEditRequested(const User &user);
    void handleDeleteRequested(const User &user);
    void requestLogout();

private:
    void setupUi();
    void enterEditMode(const User &user);
    void exitEditMode();

    QPushButton *adminLogoutButton;
    QPushButton *m_openMaterialsButton;

    UserFormWidget *m_userFormWidget;
    UsersTableWidget *m_usersTableWidget;

    bool m_isEditMode;
    QString m_editingUserNo;

    UserRepository m_userRepository;
};

#endif // USERSPAGE_H
