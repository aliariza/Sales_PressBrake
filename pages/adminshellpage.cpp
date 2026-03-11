#include "adminshellpage.h"
#include "userspage.h"
#include "materialspage.h"
#include "machinespage.h"
#include "toolingpage.h"
#include "optionspage.h"
#include "stylehelper.h"

#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QFont>
#include <QWidget>
#include <QSizePolicy>

AdminShellPage::AdminShellPage(QWidget *parent)
    : QWidget(parent),
    m_usersButton(nullptr),
    m_materialsButton(nullptr),
    m_machinesButton(nullptr),
    m_toolingButton(nullptr),
    m_optionsButton(nullptr),
    m_logoutButton(nullptr),
    m_contentStack(nullptr),
    m_dashboardPage(nullptr),
    m_dashboardTitleLabel(nullptr),
    m_dashboardInfoLabel(nullptr),
    m_usersPage(nullptr),
    m_materialsPage(nullptr),
    m_machinesPage(nullptr),
    m_toolingPage(nullptr),
    m_optionsPage(nullptr)
{
    setupUi();
    showDashboardPage();
}

void AdminShellPage::setupUi()
{
    auto *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *sidebarFrame = new QFrame(this);
    sidebarFrame->setFixedWidth(240);
    sidebarFrame->setObjectName("adminSidebar");
    sidebarFrame->setStyleSheet(
        "#adminSidebar {"
        " border-right: 1px solid #d8d8d8;"
        " background-color: #f7f7f7;"
        " }"
        );

    auto *sidebarLayout = new QVBoxLayout(sidebarFrame);
    sidebarLayout->setContentsMargins(10, 18, 10, 18);
    sidebarLayout->setSpacing(12);

    auto *titleLabel = new QLabel("ADMIN", sidebarFrame);
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    m_usersButton = new QPushButton("USERS", sidebarFrame);
    m_materialsButton = new QPushButton("MATERIALS", sidebarFrame);
    m_machinesButton = new QPushButton("MACHINES", sidebarFrame);
    m_toolingButton = new QPushButton("TOOLING", sidebarFrame);
    m_optionsButton = new QPushButton("OPTIONS", sidebarFrame);
    m_logoutButton = new QPushButton("LOGOUT", sidebarFrame);

    const QList<QPushButton*> navButtons{
        m_usersButton, m_materialsButton, m_machinesButton, m_toolingButton, m_optionsButton
    };

    for (QPushButton *button : navButtons) {
        button->setFixedHeight(42);
        button->setMinimumWidth(0);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        button->setCursor(Qt::PointingHandCursor);
        StyleHelper::applySecondaryButtonStyle(button);
    }

    m_logoutButton->setFixedHeight(42);
    m_logoutButton->setMinimumWidth(0);
    m_logoutButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_logoutButton->setCursor(Qt::PointingHandCursor);
    StyleHelper::applySecondaryButtonStyle(m_logoutButton);

    sidebarLayout->addWidget(titleLabel);
    sidebarLayout->addSpacing(8);
    sidebarLayout->addWidget(m_materialsButton);
    sidebarLayout->addWidget(m_machinesButton);
    sidebarLayout->addWidget(m_toolingButton);
    sidebarLayout->addWidget(m_optionsButton);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(m_usersButton);
    sidebarLayout->addWidget(m_logoutButton);

    m_contentStack = new QStackedWidget(this);

    m_dashboardPage = new QWidget(this);

    auto *dashboardLayout = new QVBoxLayout(m_dashboardPage);
    dashboardLayout->setContentsMargins(40, 40, 40, 40);
    dashboardLayout->setSpacing(16);

    m_dashboardTitleLabel = new QLabel("Admin Dashboard", m_dashboardPage);
    QFont dashboardTitleFont;
    dashboardTitleFont.setPointSize(24);
    dashboardTitleFont.setBold(true);
    m_dashboardTitleLabel->setFont(dashboardTitleFont);
    m_dashboardTitleLabel->setAlignment(Qt::AlignCenter);

    m_dashboardInfoLabel = new QLabel(
        "Select a section from the left menu:\nUsers, Materials, Machines, Tooling, or Options.",
        m_dashboardPage
        );
    m_dashboardInfoLabel->setAlignment(Qt::AlignCenter);
    m_dashboardInfoLabel->setWordWrap(true);

    dashboardLayout->addStretch();
    dashboardLayout->addWidget(m_dashboardTitleLabel);
    dashboardLayout->addWidget(m_dashboardInfoLabel);
    dashboardLayout->addStretch();

    m_usersPage = new UsersPage(this);
    m_materialsPage = new MaterialsPage(this);
    m_machinesPage = new MachinesPage(this);
    m_toolingPage = new ToolingPage(this);
    m_optionsPage = new OptionsPage(this);

    m_contentStack->addWidget(m_dashboardPage);
    m_contentStack->addWidget(m_usersPage);
    m_contentStack->addWidget(m_materialsPage);
    m_contentStack->addWidget(m_machinesPage);
    m_contentStack->addWidget(m_toolingPage);
    m_contentStack->addWidget(m_optionsPage);

    rootLayout->addWidget(sidebarFrame);
    rootLayout->addWidget(m_contentStack, 1);

    connect(m_usersButton, &QPushButton::clicked, this, &AdminShellPage::showUsersPage);
    connect(m_materialsButton, &QPushButton::clicked, this, &AdminShellPage::showMaterialsPage);
    connect(m_machinesButton, &QPushButton::clicked, this, &AdminShellPage::showMachinesPage);
    connect(m_toolingButton, &QPushButton::clicked, this, &AdminShellPage::showToolingPage);
    connect(m_optionsButton, &QPushButton::clicked, this, &AdminShellPage::showOptionsPage);
    connect(m_logoutButton, &QPushButton::clicked, this, &AdminShellPage::logoutRequested);

    connect(m_usersPage, &UsersPage::logoutRequested, this, &AdminShellPage::logoutRequested);
}

void AdminShellPage::updateNavStyles(QPushButton *activeButton)
{
    const QList<QPushButton*> navButtons{
        m_usersButton, m_materialsButton, m_machinesButton, m_toolingButton, m_optionsButton
    };

    for (QPushButton *button : navButtons) {
        if (button == activeButton) {
            StyleHelper::applyPrimaryButtonStyle(button);
        } else {
            StyleHelper::applySecondaryButtonStyle(button);
        }
    }

    StyleHelper::applySecondaryButtonStyle(m_logoutButton);
}

void AdminShellPage::showDashboardPage()
{
    m_contentStack->setCurrentWidget(m_dashboardPage);
    updateNavStyles(nullptr);
}

void AdminShellPage::showUsersPage()
{
    m_usersPage->refreshUserTable();
    m_contentStack->setCurrentWidget(m_usersPage);
    updateNavStyles(m_usersButton);
}

void AdminShellPage::showMaterialsPage()
{
    m_materialsPage->refreshMaterialsTable();
    m_contentStack->setCurrentWidget(m_materialsPage);
    updateNavStyles(m_materialsButton);
}

void AdminShellPage::showMachinesPage()
{
    m_machinesPage->refreshMachinesTable();
    m_contentStack->setCurrentWidget(m_machinesPage);
    updateNavStyles(m_machinesButton);
}

void AdminShellPage::showToolingPage()
{
    m_toolingPage->refreshToolingTable();
    m_contentStack->setCurrentWidget(m_toolingPage);
    updateNavStyles(m_toolingButton);
}

void AdminShellPage::showOptionsPage()
{
    m_optionsPage->refreshOptionsTable();
    m_contentStack->setCurrentWidget(m_optionsPage);
    updateNavStyles(m_optionsButton);
}
