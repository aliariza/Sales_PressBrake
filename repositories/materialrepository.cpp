#include "materialrepository.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDevice>
#include <QTextStream>
#include <QStringList>
#include <QLocale>

MaterialRepository::MaterialRepository()
    : m_defaultResourcePath(":/materials.csv")
{
    const QString appDataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    m_filePath = appDataDir + "/materials.csv";
}

bool MaterialRepository::initialize(QString &errorMessage)
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
            errorMessage = "Default materials.csv not found in Qt resources:\n" + m_defaultResourcePath;
            return false;
        }

        if (!resourceFile.copy(m_filePath)) {
            errorMessage = "Could not copy default materials.csv to writable location:\n" + m_filePath;
            return false;
        }
    }

    QFile permissionsFile(m_filePath);
    if (!permissionsFile.setPermissions(
            QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
            QFileDevice::ReadUser   | QFileDevice::WriteUser  |
            QFileDevice::ReadGroup  |
            QFileDevice::ReadOther)) {
        errorMessage = "Could not set writable permissions on materials.csv:\n" + m_filePath;
        return false;
    }

    return true;
}

std::optional<MaterialRecord> MaterialRepository::parseMaterialLine(const QString &line) const
{
    const QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty())
        return std::nullopt;

    const QStringList parts = trimmedLine.split(",");

    if (parts.size() < 9)
        return std::nullopt;

    MaterialRecord material;
    material.no                     = parts[0].trimmed();
    material.name                   = parts[1].trimmed();
    material.tensileStrengthMPa     = parts[2].trimmed();
    material.yieldStrengthMPa       = parts[3].trimmed();
    material.kFactorDefault         = parts[4].trimmed();
    material.youngsModulusMPa       = parts[5].trimmed();
    material.recommendedVdieFactor  = parts[6].trimmed();
    material.minThicknessMm         = parts[7].trimmed();
    material.maxThicknessMm         = parts[8].trimmed();

    return material;
}

QList<MaterialRecord> MaterialRepository::getAllMaterials(QString &errorMessage) const
{
    errorMessage.clear();

    QList<MaterialRecord> materials;

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable materials.csv not found:\n" + m_filePath;
        return materials;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable materials.csv:\n" + m_filePath;
        return materials;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<MaterialRecord> material = parseMaterialLine(line);

        if (!material.has_value())
            continue;

        materials.append(*material);
    }

    return materials;
}

QString MaterialRepository::getNextMaterialNo(QString &errorMessage) const
{
    errorMessage.clear();

    const QList<MaterialRecord> materials = getAllMaterials(errorMessage);
    if (!errorMessage.isEmpty()) {
        return QString();
    }

    int maxNo = 0;

    for (const MaterialRecord &material : materials) {
        bool ok = false;
        const int value = material.no.toInt(&ok);

        if (ok && value > maxNo) {
            maxNo = value;
        }
    }

    return QString::number(maxNo + 1);
}

bool MaterialRepository::addMaterial(const MaterialRecord &material, QString &errorMessage) const
{
    errorMessage.clear();

    if (!validateMaterial(material, errorMessage)) {
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        errorMessage = "Could not open writable materials.csv for appending:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);
    out << material.no.trimmed() << ","
        << material.name.trimmed() << ","
        << material.tensileStrengthMPa.trimmed() << ","
        << material.yieldStrengthMPa.trimmed() << ","
        << material.kFactorDefault.trimmed() << ","
        << material.youngsModulusMPa.trimmed() << ","
        << material.recommendedVdieFactor.trimmed() << ","
        << material.minThicknessMm.trimmed() << ","
        << material.maxThicknessMm.trimmed() << "\n";

    return true;
}

bool MaterialRepository::updateMaterial(const QString &no, const MaterialRecord &updatedMaterial, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Material number cannot be empty.";
        return false;
    }

    if (!validateMaterial(updatedMaterial, errorMessage)) {
        return false;
    }

    QList<MaterialRecord> materials = getAllMaterials(errorMessage);
    if (!errorMessage.isEmpty()) {
        return false;
    }

    bool found = false;

    for (MaterialRecord &material : materials) {
        if (material.no.trimmed() == normalizedNo) {
            material.name = updatedMaterial.name.trimmed();
            material.tensileStrengthMPa = updatedMaterial.tensileStrengthMPa.trimmed();
            material.yieldStrengthMPa = updatedMaterial.yieldStrengthMPa.trimmed();
            material.kFactorDefault = updatedMaterial.kFactorDefault.trimmed();
            material.youngsModulusMPa = updatedMaterial.youngsModulusMPa.trimmed();
            material.recommendedVdieFactor = updatedMaterial.recommendedVdieFactor.trimmed();
            material.minThicknessMm = updatedMaterial.minThicknessMm.trimmed();
            material.maxThicknessMm = updatedMaterial.maxThicknessMm.trimmed();
            found = true;
            break;
        }
    }

    if (!found) {
        errorMessage = "Material to update was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable materials.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const MaterialRecord &material : materials) {
        out << material.no.trimmed() << ","
            << material.name.trimmed() << ","
            << material.tensileStrengthMPa.trimmed() << ","
            << material.yieldStrengthMPa.trimmed() << ","
            << material.kFactorDefault.trimmed() << ","
            << material.youngsModulusMPa.trimmed() << ","
            << material.recommendedVdieFactor.trimmed() << ","
            << material.minThicknessMm.trimmed() << ","
            << material.maxThicknessMm.trimmed() << "\n";
    }

    return true;
}

bool MaterialRepository::deleteMaterial(const QString &no, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Material number cannot be empty.";
        return false;
    }

    QList<MaterialRecord> materials = getAllMaterials(errorMessage);
    if (!errorMessage.isEmpty()) {
        return false;
    }

    QList<MaterialRecord> filteredMaterials;
    bool found = false;

    for (const MaterialRecord &material : materials) {
        if (material.no.trimmed() == normalizedNo) {
            found = true;
            continue;
        }
        filteredMaterials.append(material);
    }

    if (!found) {
        errorMessage = "Material to delete was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable materials.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const MaterialRecord &material : filteredMaterials) {
        out << material.no.trimmed() << ","
            << material.name.trimmed() << ","
            << material.tensileStrengthMPa.trimmed() << ","
            << material.yieldStrengthMPa.trimmed() << ","
            << material.kFactorDefault.trimmed() << ","
            << material.youngsModulusMPa.trimmed() << ","
            << material.recommendedVdieFactor.trimmed() << ","
            << material.minThicknessMm.trimmed() << ","
            << material.maxThicknessMm.trimmed() << "\n";
    }

    return true;
}
bool MaterialRepository::validateMaterial(const MaterialRecord &material, QString &errorMessage) const
{
    errorMessage.clear();

    if (material.no.trimmed().isEmpty()) {
        errorMessage = "Material number cannot be empty.";
        return false;
    }

    if (material.name.trimmed().isEmpty()) {
        errorMessage = "Material name cannot be empty.";
        return false;
    }

    bool ok = false;

    const double tensileStrength = QLocale::c().toDouble(material.tensileStrengthMPa.trimmed(), &ok);
    if (!ok || tensileStrength <= 0.0) {
        errorMessage = "Tensile strength (Rm) must be a valid number greater than 0. Use dot as decimal separator.";
        return false;
    }

    const double yieldStrength = QLocale::c().toDouble(material.yieldStrengthMPa.trimmed(), &ok);
    if (!ok || yieldStrength <= 0.0) {
        errorMessage = "Yield strength (Re) must be a valid number greater than 0. Use dot as decimal separator.";
        return false;
    }

    const double kFactor = QLocale::c().toDouble(material.kFactorDefault.trimmed(), &ok);
    if (!ok || kFactor < 0.0 || kFactor > 1.0) {
        errorMessage = "K Factor Default must be a valid number between 0 and 1. Use dot as decimal separator, for example 0.25.";
        return false;
    }

    const double youngsModulus = QLocale::c().toDouble(material.youngsModulusMPa.trimmed(), &ok);
    if (!ok || youngsModulus <= 0.0) {
        errorMessage = "Young's Modulus (E) must be a valid number greater than 0. Use dot as decimal separator.";
        return false;
    }

    const double vdieFactor = QLocale::c().toDouble(material.recommendedVdieFactor.trimmed(), &ok);
    if (!ok || vdieFactor <= 0.0) {
        errorMessage = "Recommended V-die Factor must be a valid number greater than 0. Use dot as decimal separator.";
        return false;
    }

    const double minThickness = QLocale::c().toDouble(material.minThicknessMm.trimmed(), &ok);
    if (!ok || minThickness < 0.0) {
        errorMessage = "Minimum Thickness must be a valid number greater than or equal to 0. Use dot as decimal separator.";
        return false;
    }

    const double maxThickness = QLocale::c().toDouble(material.maxThicknessMm.trimmed(), &ok);
    if (!ok || maxThickness < 0.0) {
        errorMessage = "Maximum Thickness must be a valid number greater than or equal to 0. Use dot as decimal separator.";
        return false;
    }

    if (maxThickness < minThickness) {
        errorMessage = "Maximum Thickness must be greater than or equal to Minimum Thickness.";
        return false;
    }

    return true;
}
