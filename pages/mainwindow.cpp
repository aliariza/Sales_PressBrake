#include "mainwindow.h"
#include "welcomepage.h"
#include "loginpage.h"
#include "adminshellpage.h"
#include "recommendationpage.h"

#include <QStackedWidget>
#include <QMessageBox>
#include <optional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    stackedWidget(new QStackedWidget(this)),
    welcomePage(nullptr),
    loginPage(nullptr),
    adminShellPage(nullptr),
    recommendationPage(nullptr)
{
    setWindowTitle("Sales Press Brake");
    resize(1280, 780);

    QString errorMessage;
    if (!m_userRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    welcomePage = new WelcomePage(this);
    connect(welcomePage, &WelcomePage::loginRequested, this, &MainWindow::goToLoginPage);
    connect(welcomePage, &WelcomePage::cancelRequested, this, &MainWindow::closeApplication);

    loginPage = new LoginPage(this);
    connect(loginPage, &LoginPage::loginRequested, this, &MainWindow::tryLogin);
    connect(loginPage, &LoginPage::backRequested, this, &MainWindow::goToWelcomePage);

    adminShellPage = new AdminShellPage(this);
    connect(adminShellPage, &AdminShellPage::logoutRequested, this, &MainWindow::goToWelcomePage);

    recommendationPage = new RecommendationPage(this);
    connect(recommendationPage, &RecommendationPage::logoutRequested,
            this, &MainWindow::goToWelcomePage);

    setCentralWidget(stackedWidget);

    stackedWidget->addWidget(welcomePage);
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(adminShellPage);
    stackedWidget->addWidget(recommendationPage);

    stackedWidget->setCurrentWidget(welcomePage);
    welcomePage->focusLoginButton();
}

MainWindow::~MainWindow() = default;

void MainWindow::goToLoginPage()
{
    loginPage->clearInputs();
    setWindowTitle("Sales Press Brake - LOGIN");
    stackedWidget->setCurrentWidget(loginPage);
    loginPage->focusUsername();
}

void MainWindow::goToWelcomePage()
{
    loginPage->clearInputs();
    setWindowTitle("Sales Press Brake");
    stackedWidget->setCurrentWidget(welcomePage);
    welcomePage->focusLoginButton();
}

void MainWindow::goToAdminShellPage()
{
    setWindowTitle("Sales Press Brake - ADMIN");
    stackedWidget->setCurrentWidget(adminShellPage);
}

void MainWindow::goToRecommendationPage()
{
    setWindowTitle("Sales Press Brake - RECOMMENDATION");
    recommendationPage->refreshData();
    stackedWidget->setCurrentWidget(recommendationPage);
}

void MainWindow::tryLogin()
{
    const QString username = loginPage->username();
    const QString password = loginPage->password();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter username and password.");
        return;
    }

    QString errorMessage;
    const std::optional<User> user = m_userRepository.findUser(username, password, errorMessage);

    if (!user.has_value()) {
        QMessageBox::critical(this, "Login Failed", errorMessage);
        return;
    }

    if (user->role == "admin") {
        goToAdminShellPage();
    } else if (user->role == "user") {
        goToRecommendationPage();
    } else {
        QMessageBox::critical(this, "Role Error", "Unknown user role.");
        goToWelcomePage();
    }
}

void MainWindow::closeApplication()
{
    close();
}
