#ifndef USERFORMWIDGET_H
#define USERFORMWIDGET_H

#include <QWidget>
#include "user.h"

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;

class UserFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserFormWidget(QWidget *parent = nullptr);

    QString userNo() const;
    QString username() const;
    QString password() const;
    QString role() const;
    QString comments() const;

    void setCreateMode();
    void setEditMode(const User &user);

    void clearForm();
    void setStatusMessage(const QString &text);
    void clearStatusMessage();
    void focusUsername();

signals:
    void submitRequested();
    void cancelEditRequested();

private:
    void setupUi();

    QLineEdit *m_userNoEdit;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QComboBox *m_roleComboBox;
    QLineEdit *m_commentsEdit;

    QPushButton *m_submitButton;
    QPushButton *m_cancelEditButton;

    QLabel *m_statusLabel;
};

#endif // USERFORMWIDGET_H
