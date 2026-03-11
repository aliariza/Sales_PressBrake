#ifndef TOOLINGRECOMMENDATIONRESULT_H
#define TOOLINGRECOMMENDATIONRESULT_H

#include <QString>
#include "toolingrecord.h"

struct ToolingRecommendationResult
{
    ToolingRecord tooling;
    QString reason;
};

#endif // TOOLINGRECOMMENDATIONRESULT_H
