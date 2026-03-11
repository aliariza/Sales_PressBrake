#ifndef USERSTABLEWIDGET_H
#define USERSTABLEWIDGET_H

#include <QWidget>
#include <QList>
#include "user.h"

class QTableWidget;

class UsersTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsersTableWidget(QWidget *parent = nullptr);

    void setUsers(const QList<User> &users);

signals:
    void editRequested(const User &user);
    void deleteRequested(const User &user);

private:
    void setupUi();

    QTableWidget *m_table;
};

#endif // USERSTABLEWIDGET_H
