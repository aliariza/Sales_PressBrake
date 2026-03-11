#ifndef RECOMMENDATIONRESULT_H
#define RECOMMENDATIONRESULT_H

#include <QString>
#include "machinerecord.h"

struct RecommendationResult
{
    MachineRecord machine;
    QString reason;
};

#endif // RECOMMENDATIONRESULT_H
