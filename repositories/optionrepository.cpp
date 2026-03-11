#include "optionrepository.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDevice>
#include <QTextStream>
#include <QStringList>
#include <QLocale>

OptionRepository::OptionRepository()
    : m_defaultResourcePath(":/options.csv")
{
    const QString appDataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    m_filePath = appDataDir + "/options.csv";
}

bool OptionRepository::initialize(QString &errorMessage)
{
    errorMessage.clear();

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
            errorMessage = "Default options.csv not found in Qt resources:\n" + m_defaultResourcePath;
            return false;
        }

        if (!resourceFile.copy(m_filePath)) {
            errorMessage = "Could not copy default options.csv to writable location:\n" + m_filePath;
            return false;
        }
    }

    QFile permissionsFile(m_filePath);
    if (!permissionsFile.setPermissions(
            QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
            QFileDevice::ReadUser   | QFileDevice::WriteUser  |
            QFileDevice::ReadGroup  |
            QFileDevice::ReadOther)) {
        errorMessage = "Could not set writable permissions on options.csv:\n" + m_filePath;
        return false;
    }

    return true;
}

std::optional<OptionRecord> OptionRepository::parseOptionLine(const QString &line) const
{
    const QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty())
        return std::nullopt;

    const QStringList parts = trimmedLine.split(",");

    if (parts.size() < 4)
        return std::nullopt;

    OptionRecord option;
    option.no = parts[0].trimmed();
    option.code = parts[1].trimmed();
    option.name = parts[2].trimmed();
    option.priceUsd = parts[3].trimmed();

    return option;
}

bool OptionRepository::codeExists(const QString &code, const QString &excludeNo, QString &errorMessage) const
{
    errorMessage.clear();

    const QList<OptionRecord> options = getAllOptions(errorMessage);
    if (!errorMessage.isEmpty())
        return false;

    for (const OptionRecord &option : options) {
        if (!excludeNo.trimmed().isEmpty() && option.no.trimmed() == excludeNo.trimmed())
            continue;

        if (option.code.compare(code.trimmed(), Qt::CaseInsensitive) == 0)
            return true;
    }

    return false;
}

bool OptionRepository::validateOption(const OptionRecord &option, QString &errorMessage) const
{
    errorMessage.clear();

    if (option.no.trimmed().isEmpty()) {
        errorMessage = "Option number cannot be empty.";
        return false;
    }

    if (option.code.trimmed().isEmpty()) {
        errorMessage = "Option code cannot be empty.";
        return false;
    }

    if (option.name.trimmed().isEmpty()) {
        errorMessage = "Option name cannot be empty.";
        return false;
    }

    bool ok = false;
    const double price = QLocale::c().toDouble(option.priceUsd.trimmed(), &ok);
    if (!ok || price < 0.0) {
        errorMessage = "Price USD must be a valid number greater than or equal to 0.";
        return false;
    }

    return true;
}

QList<OptionRecord> OptionRepository::getAllOptions(QString &errorMessage) const
{
    errorMessage.clear();

    QList<OptionRecord> options;

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable options.csv not found:\n" + m_filePath;
        return options;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable options.csv:\n" + m_filePath;
        return options;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<OptionRecord> option = parseOptionLine(line);

        if (!option.has_value())
            continue;

        options.append(*option);
    }

    return options;
}

QString OptionRepository::getNextOptionNo(QString &errorMessage) const
{
    errorMessage.clear();

    const QList<OptionRecord> options = getAllOptions(errorMessage);
    if (!errorMessage.isEmpty())
        return QString();

    int maxNo = 0;

    for (const OptionRecord &option : options) {
        bool ok = false;
        const int value = option.no.toInt(&ok);

        if (ok && value > maxNo)
            maxNo = value;
    }

    return QString::number(maxNo + 1);
}

bool OptionRepository::addOption(const OptionRecord &option, QString &errorMessage) const
{
    errorMessage.clear();

    if (!validateOption(option, errorMessage))
        return false;

    if (codeExists(option.code, "", errorMessage)) {
        errorMessage = "Option code already exists.";
        return false;
    }

    if (!errorMessage.isEmpty())
        return false;

    QFile file(m_filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        errorMessage = "Could not open writable options.csv for appending:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);
    out << option.no.trimmed() << ","
        << option.code.trimmed() << ","
        << option.name.trimmed() << ","
        << option.priceUsd.trimmed() << "\n";

    return true;
}

bool OptionRepository::updateOption(const QString &no, const OptionRecord &updatedOption, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Option number cannot be empty.";
        return false;
    }

    if (!validateOption(updatedOption, errorMessage))
        return false;

    if (codeExists(updatedOption.code, normalizedNo, errorMessage)) {
        errorMessage = "Option code already exists.";
        return false;
    }

    if (!errorMessage.isEmpty())
        return false;

    QList<OptionRecord> options = getAllOptions(errorMessage);
    if (!errorMessage.isEmpty())
        return false;

    bool found = false;

    for (OptionRecord &option : options) {
        if (option.no.trimmed() == normalizedNo) {
            option.code = updatedOption.code.trimmed();
            option.name = updatedOption.name.trimmed();
            option.priceUsd = updatedOption.priceUsd.trimmed();
            found = true;
            break;
        }
    }

    if (!found) {
        errorMessage = "Option to update was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable options.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const OptionRecord &option : options) {
        out << option.no.trimmed() << ","
            << option.code.trimmed() << ","
            << option.name.trimmed() << ","
            << option.priceUsd.trimmed() << "\n";
    }

    return true;
}

bool OptionRepository::deleteOption(const QString &no, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Option number cannot be empty.";
        return false;
    }

    QList<OptionRecord> options = getAllOptions(errorMessage);
    if (!errorMessage.isEmpty())
        return false;

    QList<OptionRecord> filteredOptions;
    bool found = false;

    for (const OptionRecord &option : options) {
        if (option.no.trimmed() == normalizedNo) {
            found = true;
            continue;
        }
        filteredOptions.append(option);
    }

    if (!found) {
        errorMessage = "Option to delete was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable options.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const OptionRecord &option : filteredOptions) {
        out << option.no.trimmed() << ","
            << option.code.trimmed() << ","
            << option.name.trimmed() << ","
            << option.priceUsd.trimmed() << "\n";
    }

    return true;
}
