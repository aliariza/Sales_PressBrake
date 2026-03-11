#ifndef MACHINERECORD_H
#define MACHINERECORD_H

#include <QString>

struct MachineRecord
{
    QString no;
    QString model;
    QString maxTonnageTonf;
    QString workingLengthMm;
    QString maxThicknessMm;
    QString basePriceUSD;
};

#endif // MACHINERECORD_H
