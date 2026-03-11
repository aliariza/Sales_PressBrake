#include "machinerepository.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDevice>
#include <QTextStream>
#include <QStringList>
#include <QLocale>

MachineRepository::MachineRepository()
    : m_defaultResourcePath(":/machines.csv")
{
    const QString appDataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    m_filePath = appDataDir + "/machines.csv";
}

bool MachineRepository::initialize(QString &errorMessage)
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
            errorMessage = "Default machines.csv not found in Qt resources:\n" + m_defaultResourcePath;
            return false;
        }

        if (!resourceFile.copy(m_filePath)) {
            errorMessage = "Could not copy default machines.csv to writable location:\n" + m_filePath;
            return false;
        }
    }

    QFile permissionsFile(m_filePath);
    if (!permissionsFile.setPermissions(
            QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
            QFileDevice::ReadUser   | QFileDevice::WriteUser  |
            QFileDevice::ReadGroup  |
            QFileDevice::ReadOther)) {
        errorMessage = "Could not set writable permissions on machines.csv:\n" + m_filePath;
        return false;
    }

    return true;
}

std::optional<MachineRecord> MachineRepository::parseMachineLine(const QString &line) const
{
    const QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty())
        return std::nullopt;

    const QStringList parts = trimmedLine.split(",");

    if (parts.size() < 6)
        return std::nullopt;

    MachineRecord machine;
    machine.no              = parts[0].trimmed();
    machine.model           = parts[1].trimmed();
    machine.maxTonnageTonf  = parts[2].trimmed();
    machine.workingLengthMm = parts[3].trimmed();
    machine.maxThicknessMm  = parts[4].trimmed();
    machine.basePriceUSD    = parts[5].trimmed();

    return machine;
}

bool MachineRepository::validateMachine(const MachineRecord &machine, QString &errorMessage) const
{
    errorMessage.clear();

    if (machine.no.trimmed().isEmpty()) {
        errorMessage = "Machine number cannot be empty.";
        return false;
    }

    if (machine.model.trimmed().isEmpty()) {
        errorMessage = "Machine model cannot be empty.";
        return false;
    }

    bool ok = false;

    const double tonnage = QLocale::c().toDouble(machine.maxTonnageTonf.trimmed(), &ok);
    if (!ok || tonnage <= 0.0) {
        errorMessage = "Max Tonnage must be a valid number greater than 0.";
        return false;
    }

    const double length = QLocale::c().toDouble(machine.workingLengthMm.trimmed(), &ok);
    if (!ok || length <= 0.0) {
        errorMessage = "Working Length must be a valid number greater than 0.";
        return false;
    }

    const double thickness = QLocale::c().toDouble(machine.maxThicknessMm.trimmed(), &ok);
    if (!ok || thickness <= 0.0) {
        errorMessage = "Max Thickness must be a valid number greater than 0.";
        return false;
    }

    const double price = QLocale::c().toDouble(machine.basePriceUSD.trimmed(), &ok);
    if (!ok || price < 0.0) {
        errorMessage = "Base Price USD must be a valid number greater than or equal to 0.";
        return false;
    }

    return true;
}

QList<MachineRecord> MachineRepository::getAllMachines(QString &errorMessage) const
{
    errorMessage.clear();

    QList<MachineRecord> machines;

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable machines.csv not found:\n" + m_filePath;
        return machines;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable machines.csv:\n" + m_filePath;
        return machines;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<MachineRecord> machine = parseMachineLine(line);

        if (!machine.has_value())
            continue;

        machines.append(*machine);
    }

    return machines;
}

QString MachineRepository::getNextMachineNo(QString &errorMessage) const
{
    errorMessage.clear();

    const QList<MachineRecord> machines = getAllMachines(errorMessage);
    if (!errorMessage.isEmpty()) {
        return QString();
    }

    int maxNo = 0;

    for (const MachineRecord &machine : machines) {
        bool ok = false;
        const int value = machine.no.toInt(&ok);

        if (ok && value > maxNo) {
            maxNo = value;
        }
    }

    return QString::number(maxNo + 1);
}

bool MachineRepository::addMachine(const MachineRecord &machine, QString &errorMessage) const
{
    errorMessage.clear();

    if (!validateMachine(machine, errorMessage)) {
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        errorMessage = "Could not open writable machines.csv for appending:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);
    out << machine.no.trimmed() << ","
        << machine.model.trimmed() << ","
        << machine.maxTonnageTonf.trimmed() << ","
        << machine.workingLengthMm.trimmed() << ","
        << machine.maxThicknessMm.trimmed() << ","
        << machine.basePriceUSD.trimmed() << "\n";

    return true;
}

bool MachineRepository::updateMachine(const QString &no, const MachineRecord &updatedMachine, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Machine number cannot be empty.";
        return false;
    }

    if (!validateMachine(updatedMachine, errorMessage)) {
        return false;
    }

    QList<MachineRecord> machines = getAllMachines(errorMessage);
    if (!errorMessage.isEmpty()) {
        return false;
    }

    bool found = false;

    for (MachineRecord &machine : machines) {
        if (machine.no.trimmed() == normalizedNo) {
            machine.model = updatedMachine.model.trimmed();
            machine.maxTonnageTonf = updatedMachine.maxTonnageTonf.trimmed();
            machine.workingLengthMm = updatedMachine.workingLengthMm.trimmed();
            machine.maxThicknessMm = updatedMachine.maxThicknessMm.trimmed();
            machine.basePriceUSD = updatedMachine.basePriceUSD.trimmed();
            found = true;
            break;
        }
    }

    if (!found) {
        errorMessage = "Machine to update was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable machines.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const MachineRecord &machine : machines) {
        out << machine.no.trimmed() << ","
            << machine.model.trimmed() << ","
            << machine.maxTonnageTonf.trimmed() << ","
            << machine.workingLengthMm.trimmed() << ","
            << machine.maxThicknessMm.trimmed() << ","
            << machine.basePriceUSD.trimmed() << "\n";
    }

    return true;
}

bool MachineRepository::deleteMachine(const QString &no, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Machine number cannot be empty.";
        return false;
    }

    QList<MachineRecord> machines = getAllMachines(errorMessage);
    if (!errorMessage.isEmpty()) {
        return false;
    }

    QList<MachineRecord> filteredMachines;
    bool found = false;

    for (const MachineRecord &machine : machines) {
        if (machine.no.trimmed() == normalizedNo) {
            found = true;
            continue;
        }
        filteredMachines.append(machine);
    }

    if (!found) {
        errorMessage = "Machine to delete was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable machines.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const MachineRecord &machine : filteredMachines) {
        out << machine.no.trimmed() << ","
            << machine.model.trimmed() << ","
            << machine.maxTonnageTonf.trimmed() << ","
            << machine.workingLengthMm.trimmed() << ","
            << machine.maxThicknessMm.trimmed() << ","
            << machine.basePriceUSD.trimmed() << "\n";
    }

    return true;
}
