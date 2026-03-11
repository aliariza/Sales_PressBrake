#ifndef MACHINEREPOSITORY_H
#define MACHINEREPOSITORY_H

#include <QString>
#include <QList>
#include <optional>
#include "machinerecord.h"

class MachineRepository
{
public:
    MachineRepository();

    bool initialize(QString &errorMessage);

    QList<MachineRecord> getAllMachines(QString &errorMessage) const;
    QString getNextMachineNo(QString &errorMessage) const;

    bool addMachine(const MachineRecord &machine, QString &errorMessage) const;
    bool updateMachine(const QString &no, const MachineRecord &updatedMachine, QString &errorMessage) const;
    bool deleteMachine(const QString &no, QString &errorMessage) const;

private:
    std::optional<MachineRecord> parseMachineLine(const QString &line) const;
    bool validateMachine(const MachineRecord &machine, QString &errorMessage) const;

    QString m_defaultResourcePath;
    QString m_filePath;
};

#endif // MACHINEREPOSITORY_H
