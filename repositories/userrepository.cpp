#include "userrepository.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QFileDevice>

UserRepository::UserRepository()
    : m_defaultResourcePath(":/users.csv")
{
    const QString appDataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    m_filePath = appDataDir + "/users.csv";
}

bool UserRepository::initialize(QString &errorMessage)
{
    errorMessage.clear();
    qDebug() << "Exists:" << QFile::exists(m_filePath);
    qDebug() << "Writable:" << QFileInfo(m_filePath).isWritable();
    qDebug() << "Path:" << m_filePath;
    const QFileInfo fileInfo(m_filePath);
    const QString dirPath = fileInfo.absolutePath();

    QDir dir;
    if (!dir.exists(dirPath)) {
        if (!dir.mkpath(dirPath)) {
            errorMessage = "Could not create app data directory:\n" + dirPath;
            return false;
        }
    }

    QFile writableFile(m_filePath);

    if (!writableFile.exists()) {
        QFile resourceFile(m_defaultResourcePath);

        if (!resourceFile.exists()) {
            errorMessage = "Default users.csv not found in Qt resources:\n" + m_defaultResourcePath;
            return false;
        }

        if (!resourceFile.copy(m_filePath)) {
            errorMessage = "Could not copy default users.csv to writable location:\n" + m_filePath;
            return false;
        }
    }

    QFile permissionsFile(m_filePath);
    if (!permissionsFile.setPermissions(
            QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
            QFileDevice::ReadUser   | QFileDevice::WriteUser  |
            QFileDevice::ReadGroup  |
            QFileDevice::ReadOther)) {
        errorMessage = "Could not set writable permissions on users.csv:\n" + m_filePath;
        return false;
    }

    return true;
}

std::optional<User> UserRepository::parseUserLine(const QString &line) const
{
    const QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty())
        return std::nullopt;

    const QStringList parts = trimmedLine.split(",");

    if (parts.size() < 5)
        return std::nullopt;

    User user;
    user.userno   = parts[0].trimmed();
    user.username = parts[1].trimmed();
    user.password = parts[2].trimmed();
    user.role     = parts[3].trimmed();
    user.comments = parts[4].trimmed();

    return user;
}

std::optional<User> UserRepository::findUser(const QString &username,
                                             const QString &password,
                                             QString &errorMessage) const
{
    errorMessage.clear();

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable users.csv not found:\n" + m_filePath;
        return std::nullopt;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable users.csv:\n" + m_filePath;
        return std::nullopt;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<User> user = parseUserLine(line);

        if (!user.has_value())
            continue;

        if (user->username == username && user->password == password) {
            return user;
        }
    }

    errorMessage = "Invalid username or password.";
    return std::nullopt;
}

bool UserRepository::userExists(const QString &username, QString &errorMessage) const
{
    errorMessage.clear();

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable users.csv not found:\n" + m_filePath;
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable users.csv:\n" + m_filePath;
        return false;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<User> user = parseUserLine(line);

        if (!user.has_value())
            continue;

        if (user->username.compare(username, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }

    return false;
}

bool UserRepository::addUser(const User &user, QString &errorMessage) const
{
    errorMessage.clear();

    if (user.userno.trimmed().isEmpty()) {
        errorMessage = "User number cannot be empty.";
        return false;
    }

    if (user.username.trimmed().isEmpty()) {
        errorMessage = "Username cannot be empty.";
        return false;
    }

    if (user.password.isEmpty()) {
        errorMessage = "Password cannot be empty.";
        return false;
    }

    if (user.role.trimmed().isEmpty()) {
        errorMessage = "Role cannot be empty.";
        return false;
    }

    if (user.role != "admin" && user.role != "user") {
        errorMessage = "Role must be either 'admin' or 'user'.";
        return false;
    }

    QString existsError;
    if (userExists(user.username, existsError)) {
        errorMessage = "User already exists.";
        return false;
    }

    if (!existsError.isEmpty()) {
        errorMessage = existsError;
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        errorMessage = "Could not open writable users.csv for appending:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);
    out << user.userno.trimmed() << ","
        << user.username.trimmed() << ","
        << user.password << ","
        << user.role.trimmed() << ","
        << user.comments.trimmed() << "\n";

    return true;
}
QList<User> UserRepository::getAllUsers(QString &errorMessage) const
{
    errorMessage.clear();

    QList<User> users;

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable users.csv not found:\n" + m_filePath;
        return users;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable users.csv:\n" + m_filePath;
        return users;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<User> user = parseUserLine(line);

        if (!user.has_value())
            continue;

        users.append(*user);
    }

    return users;
}

bool UserRepository::deleteUser(const QString &username, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedUsername = username.trimmed();

    if (normalizedUsername.isEmpty()) {
        errorMessage = "Username cannot be empty.";
        return false;
    }

    if (isLastAdmin(normalizedUsername, errorMessage)) {
        errorMessage = "Cannot delete the last remaining admin user.";
        return false;
    }

    if (!errorMessage.isEmpty()) {
        return false;
    }

    QString readError;
    const QList<User> users = getAllUsers(readError);

    if (!readError.isEmpty()) {
        errorMessage = readError;
        return false;
    }

    QList<User> filteredUsers;
    bool found = false;

    for (const User &user : users) {
        if (user.username.trimmed().compare(normalizedUsername, Qt::CaseInsensitive) == 0) {
            found = true;
            continue;
        }
        filteredUsers.append(user);
    }

    if (!found) {
        errorMessage = "Selected user was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable users.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const User &user : filteredUsers) {
        out << user.userno.trimmed() << ","
            << user.username.trimmed() << ","
            << user.password << ","
            << user.role.trimmed() << ","
            << user.comments.trimmed() << "\n";
    }

    return true;
}
bool UserRepository::isLastAdmin(const QString &username, QString &errorMessage) const
{
    errorMessage.clear();

    const QList<User> users = getAllUsers(errorMessage);
    if (!errorMessage.isEmpty()) {
        return false;
    }

    int adminCount = 0;
    bool targetIsAdmin = false;

    for (const User &user : users) {
        if (user.role.compare("admin", Qt::CaseInsensitive) == 0) {
            ++adminCount;

            if (user.username.compare(username, Qt::CaseInsensitive) == 0) {
                targetIsAdmin = true;
            }
        }
    }

    if (!targetIsAdmin) {
        return false;
    }

    return adminCount == 1;
}
QString UserRepository::getNextUserNo(QString &errorMessage) const
{
    errorMessage.clear();

    const QList<User> users = getAllUsers(errorMessage);
    if (!errorMessage.isEmpty()) {
        return QString();
    }

    int maxNo = 0;

    for (const User &user : users) {
        bool ok = false;
        const int value = user.userno.toInt(&ok);

        if (ok && value > maxNo) {
            maxNo = value;
        }
    }

    return QString::number(maxNo + 1);
}
bool UserRepository::updateUser(const QString &userno, const User &updatedUser, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedUserNo = userno.trimmed();

    if (normalizedUserNo.isEmpty()) {
        errorMessage = "User number cannot be empty.";
        return false;
    }

    if (updatedUser.username.trimmed().isEmpty()) {
        errorMessage = "Username cannot be empty.";
        return false;
    }

    if (updatedUser.role.trimmed().isEmpty()) {
        errorMessage = "Role cannot be empty.";
        return false;
    }

    if (updatedUser.role != "admin" && updatedUser.role != "user") {
        errorMessage = "Role must be either 'admin' or 'user'.";
        return false;
    }

    QString readError;
    QList<User> users = getAllUsers(readError);

    if (!readError.isEmpty()) {
        errorMessage = readError;
        return false;
    }

    bool found = false;

    for (User &user : users) {
        if (user.userno.trimmed() == normalizedUserNo) {
            user.username = updatedUser.username.trimmed();
            user.role = updatedUser.role.trimmed();
            user.comments = updatedUser.comments.trimmed();
            found = true;
            break;
        }
    }

    if (!found) {
        errorMessage = "User to update was not found.";
        return false;
    }

    int sameUsernameCount = 0;
    for (const User &user : users) {
        if (user.username.compare(updatedUser.username.trimmed(), Qt::CaseInsensitive) == 0) {
            ++sameUsernameCount;
        }
    }

    if (sameUsernameCount > 1) {
        errorMessage = "Another user already has this username.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable users.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const User &user : users) {
        out << user.userno.trimmed() << ","
            << user.username.trimmed() << ","
            << user.password << ","
            << user.role.trimmed() << ","
            << user.comments.trimmed() << "\n";
    }

    return true;
}
