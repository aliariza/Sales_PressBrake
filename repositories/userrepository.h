#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include <QString>
#include <QList>
#include <optional>
#include "user.h"

class UserRepository
{
public:
    UserRepository();

    bool initialize(QString &errorMessage);

    std::optional<User> findUser(const QString &username,
                                 const QString &password,
                                 QString &errorMessage) const;

    bool userExists(const QString &username, QString &errorMessage) const;
    bool addUser(const User &user, QString &errorMessage) const;
    bool deleteUser(const QString &username, QString &errorMessage) const;
    bool isLastAdmin(const QString &username, QString &errorMessage) const;
    bool updateUser(const QString &userno, const User &updatedUser, QString &errorMessage) const;

    QList<User> getAllUsers(QString &errorMessage) const;
    QString getNextUserNo(QString &errorMessage) const;

private:
    std::optional<User> parseUserLine(const QString &line) const;

    QString m_defaultResourcePath;
    QString m_filePath;
};

#endif // USERREPOSITORY_H
