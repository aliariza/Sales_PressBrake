#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "userrepository.h"

class QStackedWidget;
class LoginPage;
class WelcomePage;
class AdminShellPage;
class UserShellPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void goToLoginPage();
    void goToWelcomePage();
    void goToAdminShellPage();
    void goToUserShellPage();
    void tryLogin();
    void closeApplication();

private:
    QStackedWidget *stackedWidget;

    WelcomePage *welcomePage;
    LoginPage *loginPage;
    AdminShellPage *adminShellPage;
    UserShellPage *userShellPage;

    UserRepository m_userRepository;
};

#endif // MAINWINDOW_H
