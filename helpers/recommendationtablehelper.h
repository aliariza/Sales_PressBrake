#ifndef RECOMMENDATIONTABLEHELPER_H
#define RECOMMENDATIONTABLEHELPER_H

#include <QList>

#include "recommendationresult.h"
#include "toolingrecommendationresult.h"
#include "optionrecord.h"

class QTableWidget;

namespace RecommendationTableHelper
{
void fillMachineResultsTable(QTableWidget *table,
                             const QList<RecommendationResult> &results);

void fillToolingResultsTable(QTableWidget *table,
                             const QList<ToolingRecommendationResult> &results);

void fillOptionsTable(QTableWidget *table,
                      const QList<OptionRecord> &options);
}

#endif // RECOMMENDATIONTABLEHELPER_H