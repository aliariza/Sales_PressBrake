#include "toolingrepository.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDevice>
#include <QTextStream>
#include <QStringList>
#include <QLocale>

ToolingRepository::ToolingRepository()
    : m_defaultResourcePath(":/tooling.csv")
{
    const QString appDataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    m_filePath = appDataDir + "/tooling.csv";
}

bool ToolingRepository::initialize(QString &errorMessage)
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
            errorMessage = "Default tooling.csv not found in Qt resources:\n" + m_defaultResourcePath;
            return false;
        }

        if (!resourceFile.copy(m_filePath)) {
            errorMessage = "Could not copy default tooling.csv to writable location:\n" + m_filePath;
            return false;
        }
    }

    QFile permissionsFile(m_filePath);
    if (!permissionsFile.setPermissions(
            QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
            QFileDevice::ReadUser   | QFileDevice::WriteUser  |
            QFileDevice::ReadGroup  |
            QFileDevice::ReadOther)) {
        errorMessage = "Could not set writable permissions on tooling.csv:\n" + m_filePath;
        return false;
    }

    return true;
}

std::optional<ToolingRecord> ToolingRepository::parseToolingLine(const QString &line) const
{
    const QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty())
        return std::nullopt;

    const QStringList parts = trimmedLine.split(",");

    if (parts.size() < 5)
        return std::nullopt;

    ToolingRecord tooling;
    tooling.no = parts[0].trimmed();
    tooling.name = parts[1].trimmed();
    tooling.vDieMm = parts[2].trimmed();
    tooling.punchRadiusMm = parts[3].trimmed();
    tooling.dieRadiusMm = parts[4].trimmed();

    return tooling;
}

bool ToolingRepository::validateTooling(const ToolingRecord &tooling, QString &errorMessage) const
{
    errorMessage.clear();

    if (tooling.no.trimmed().isEmpty()) {
        errorMessage = "Tooling number cannot be empty.";
        return false;
    }

    if (tooling.name.trimmed().isEmpty()) {
        errorMessage = "Tooling name cannot be empty.";
        return false;
    }

    bool ok = false;

    const double vDie = QLocale::c().toDouble(tooling.vDieMm.trimmed(), &ok);
    if (!ok || vDie <= 0.0) {
        errorMessage = "V-die must be a valid number greater than 0.";
        return false;
    }

    const double punchRadius = QLocale::c().toDouble(tooling.punchRadiusMm.trimmed(), &ok);
    if (!ok || punchRadius < 0.0) {
        errorMessage = "Punch Radius must be a valid number greater than or equal to 0.";
        return false;
    }

    const double dieRadius = QLocale::c().toDouble(tooling.dieRadiusMm.trimmed(), &ok);
    if (!ok || dieRadius < 0.0) {
        errorMessage = "Die Radius must be a valid number greater than or equal to 0.";
        return false;
    }

    return true;
}

QList<ToolingRecord> ToolingRepository::getAllTooling(QString &errorMessage) const
{
    errorMessage.clear();

    QList<ToolingRecord> toolingList;

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable tooling.csv not found:\n" + m_filePath;
        return toolingList;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable tooling.csv:\n" + m_filePath;
        return toolingList;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<ToolingRecord> tooling = parseToolingLine(line);

        if (!tooling.has_value())
            continue;

        toolingList.append(*tooling);
    }

    return toolingList;
}

QString ToolingRepository::getNextToolingNo(QString &errorMessage) const
{
    errorMessage.clear();

    const QList<ToolingRecord> toolingList = getAllTooling(errorMessage);
    if (!errorMessage.isEmpty())
        return QString();

    int maxNo = 0;

    for (const ToolingRecord &tooling : toolingList) {
        bool ok = false;
        const int value = tooling.no.toInt(&ok);

        if (ok && value > maxNo)
            maxNo = value;
    }

    return QString::number(maxNo + 1);
}

bool ToolingRepository::addTooling(const ToolingRecord &tooling, QString &errorMessage) const
{
    errorMessage.clear();

    if (!validateTooling(tooling, errorMessage))
        return false;

    QFile file(m_filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        errorMessage = "Could not open writable tooling.csv for appending:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);
    out << tooling.no.trimmed() << ","
        << tooling.name.trimmed() << ","
        << tooling.vDieMm.trimmed() << ","
        << tooling.punchRadiusMm.trimmed() << ","
        << tooling.dieRadiusMm.trimmed() << "\n";

    return true;
}

bool ToolingRepository::updateTooling(const QString &no, const ToolingRecord &updatedTooling, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Tooling number cannot be empty.";
        return false;
    }

    if (!validateTooling(updatedTooling, errorMessage))
        return false;

    QList<ToolingRecord> toolingList = getAllTooling(errorMessage);
    if (!errorMessage.isEmpty())
        return false;

    bool found = false;

    for (ToolingRecord &tooling : toolingList) {
        if (tooling.no.trimmed() == normalizedNo) {
            tooling.name = updatedTooling.name.trimmed();
            tooling.vDieMm = updatedTooling.vDieMm.trimmed();
            tooling.punchRadiusMm = updatedTooling.punchRadiusMm.trimmed();
            tooling.dieRadiusMm = updatedTooling.dieRadiusMm.trimmed();
            found = true;
            break;
        }
    }

    if (!found) {
        errorMessage = "Tooling to update was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable tooling.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const ToolingRecord &tooling : toolingList) {
        out << tooling.no.trimmed() << ","
            << tooling.name.trimmed() << ","
            << tooling.vDieMm.trimmed() << ","
            << tooling.punchRadiusMm.trimmed() << ","
            << tooling.dieRadiusMm.trimmed() << "\n";
    }

    return true;
}

bool ToolingRepository::deleteTooling(const QString &no, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Tooling number cannot be empty.";
        return false;
    }

    QList<ToolingRecord> toolingList = getAllTooling(errorMessage);
    if (!errorMessage.isEmpty())
        return false;

    QList<ToolingRecord> filteredTooling;
    bool found = false;

    for (const ToolingRecord &tooling : toolingList) {
        if (tooling.no.trimmed() == normalizedNo) {
            found = true;
            continue;
        }
        filteredTooling.append(tooling);
    }

    if (!found) {
        errorMessage = "Tooling to delete was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable tooling.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const ToolingRecord &tooling : filteredTooling) {
        out << tooling.no.trimmed() << ","
            << tooling.name.trimmed() << ","
            << tooling.vDieMm.trimmed() << ","
            << tooling.punchRadiusMm.trimmed() << ","
            << tooling.dieRadiusMm.trimmed() << "\n";
    }

    return true;
}
