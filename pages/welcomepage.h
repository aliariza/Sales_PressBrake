#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;
class QLineEdit;

class WelcomePage : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomePage(QWidget *parent = nullptr);
    void focusLoginButton();

signals:
    void loginRequested();
    void cancelRequested();

private:
    void setupUi();

    QLabel *titleLabel;
    QPushButton *welcomeLoginButton;
    QPushButton *welcomeCancelButton;
};

#endif // WELCOMEPAGE_H
