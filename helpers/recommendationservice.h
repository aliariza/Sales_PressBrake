#ifndef RECOMMENDATIONSERVICE_H
#define RECOMMENDATIONSERVICE_H

#include <QList>
#include <QString>

#include "recommendationresult.h"
#include "toolingrecommendationresult.h"

class MachineRepository;
class ToolingRepository;

class RecommendationService
{
public:
    RecommendationService(const MachineRepository *machineRepository,
                          const ToolingRepository *toolingRepository);

    QList<RecommendationResult> buildMachineRecommendations(const QString &materialName,
                                                            double thickness,
                                                            double bendLength,
                                                            QString &errorMessage) const;

    QList<ToolingRecommendationResult> buildToolingRecommendations(double thickness,
                                                                   QString &errorMessage) const;

private:
    const MachineRepository *m_machineRepository;
    const ToolingRepository *m_toolingRepository;
};

#endif // RECOMMENDATIONSERVICE_H