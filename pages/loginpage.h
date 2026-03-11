#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget.h>

class QLabel;
class QPushButton;
class QLineEdit;

class LoginPage : public QWidget
{
    Q_OBJECT
public:
    explicit LoginPage(QWidget *parent = nullptr);
    QString username() const;
    QString password() const;
    void clearInputs();
    void focusUsername();

signals:
    void loginRequested();
    void backRequested();

private:
    void setupUi();

    QLabel *titleLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *backButton;
};

#endif // LOGINPAGE_H
