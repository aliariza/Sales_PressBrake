#ifndef MATERIALRECORD_H
#define MATERIALRECORD_H

#include <QString>

struct MaterialRecord
{
    QString no;
    QString name;
    QString tensileStrengthMPa;
    QString yieldStrengthMPa;
    QString kFactorDefault;
    QString youngsModulusMPa;
    QString recommendedVdieFactor;
    QString minThicknessMm;
    QString maxThicknessMm;
};

#endif // MATERIALRECORD_H
