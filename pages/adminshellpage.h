#ifndef ADMINSHELLPAGE_H
#define ADMINSHELLPAGE_H

#include <QWidget>

class QPushButton;
class QStackedWidget;
class QLabel;
class QWidget;
class UsersPage;
class MaterialsPage;
class MachinesPage;
class ToolingPage;
class OptionsPage;

class AdminShellPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminShellPage(QWidget *parent = nullptr);

    void showDashboardPage();
    void showUsersPage();
    void showMaterialsPage();
    void showMachinesPage();
    void showToolingPage();
    void showOptionsPage();

signals:
    void logoutRequested();

private:
    void setupUi();
    void updateNavStyles(QPushButton *activeButton);

    QPushButton *m_usersButton;
    QPushButton *m_materialsButton;
    QPushButton *m_machinesButton;
    QPushButton *m_toolingButton;
    QPushButton *m_optionsButton;
    QPushButton *m_logoutButton;

    QStackedWidget *m_contentStack;

    QWidget *m_dashboardPage;
    QLabel *m_dashboardTitleLabel;
    QLabel *m_dashboardInfoLabel;

    UsersPage *m_usersPage;
    MaterialsPage *m_materialsPage;
    MachinesPage *m_machinesPage;
    ToolingPage *m_toolingPage;
    OptionsPage *m_optionsPage;
};

#endif // ADMINSHELLPAGE_H
