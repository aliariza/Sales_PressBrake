#ifndef MATERIALREPOSITORY_H
#define MATERIALREPOSITORY_H

#include <QString>
#include <QList>
#include <optional>
#include "materialrecord.h"

class MaterialRepository
{
public:
    MaterialRepository();

    bool initialize(QString &errorMessage);

    QList<MaterialRecord> getAllMaterials(QString &errorMessage) const;
    QString getNextMaterialNo(QString &errorMessage) const;

    bool addMaterial(const MaterialRecord &material, QString &errorMessage) const;
    bool updateMaterial(const QString &no, const MaterialRecord &updatedMaterial, QString &errorMessage) const;
    bool deleteMaterial(const QString &no, QString &errorMessage) const;

private:
    std::optional<MaterialRecord> parseMaterialLine(const QString &line) const;
    bool validateMaterial(const MaterialRecord &material, QString &errorMessage) const;

    QString m_defaultResourcePath;
    QString m_filePath;
};

#endif // MATERIALREPOSITORY_H
