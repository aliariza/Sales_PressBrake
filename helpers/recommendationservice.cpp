#include "recommendationservice.h"
#include "recommendationformatutils.h"

#include "machinerepository.h"
#include "toolingrepository.h"

#include <QLocale>
#include <algorithm>
#include <cmath>

RecommendationService::RecommendationService(const MachineRepository *machineRepository,
                                             const ToolingRepository *toolingRepository)
    : m_machineRepository(machineRepository),
    m_toolingRepository(toolingRepository)
{
}

QList<RecommendationResult> RecommendationService::buildMachineRecommendations(const QString &materialName,
                                                                               double thickness,
                                                                               double bendLength,
                                                                               QString &errorMessage) const
{
    Q_UNUSED(materialName)

    QList<RecommendationResult> results;
    errorMessage.clear();

    if (!m_machineRepository) {
        errorMessage = "Makine deposu kullanılamıyor.";
        return results;
    }

    const QList<MachineRecord> machines = m_machineRepository->getAllMachines(errorMessage);
    if (!errorMessage.isEmpty())
        return results;

    for (const MachineRecord &machine : machines) {
        bool thicknessOk = false;
        bool lengthOk = false;

        const double machineMaxThickness =
            QLocale::c().toDouble(machine.maxThicknessMm.trimmed(), &thicknessOk);
        const double machineWorkingLength =
            QLocale::c().toDouble(machine.workingLengthMm.trimmed(), &lengthOk);

        if (!thicknessOk || !lengthOk)
            continue;

        if (machineMaxThickness >= thickness && machineWorkingLength >= bendLength) {
            RecommendationResult result;
            result.machine = machine;
            result.reason =
                "Uygundur; çünkü azami kalınlık (" + machine.maxThicknessMm +
                " mm) ve çalışma boyu (" + machine.workingLengthMm +
                " mm) gereksinimi karşılamaktadır.";
            results.append(result);
        }
    }

    std::sort(results.begin(), results.end(),
              [thickness, bendLength](const RecommendationResult &a,
                                      const RecommendationResult &b) {
                  bool okA1 = false, okA2 = false, okA3 = false;
                  bool okB1 = false, okB2 = false, okB3 = false;

                  const double aThickness = QLocale::c().toDouble(a.machine.maxThicknessMm, &okA1);
                  const double aLength = QLocale::c().toDouble(a.machine.workingLengthMm, &okA2);
                  const double aPrice = QLocale::c().toDouble(a.machine.basePriceUSD, &okA3);

                  const double bThickness = QLocale::c().toDouble(b.machine.maxThicknessMm, &okB1);
                  const double bLength = QLocale::c().toDouble(b.machine.workingLengthMm, &okB2);
                  const double bPrice = QLocale::c().toDouble(b.machine.basePriceUSD, &okB3);

                  const double aThicknessExcess = okA1 ? (aThickness - thickness) : 1e12;
                  const double bThicknessExcess = okB1 ? (bThickness - thickness) : 1e12;

                  if (aThicknessExcess != bThicknessExcess)
                      return aThicknessExcess < bThicknessExcess;

                  const double aLengthExcess = okA2 ? (aLength - bendLength) : 1e12;
                  const double bLengthExcess = okB2 ? (bLength - bendLength) : 1e12;

                  if (aLengthExcess != bLengthExcess)
                      return aLengthExcess < bLengthExcess;

                  return (okA3 ? aPrice : 1e12) < (okB3 ? bPrice : 1e12);
              });

    return results;
}

QList<ToolingRecommendationResult> RecommendationService::buildToolingRecommendations(double thickness,
                                                                                      QString &errorMessage) const
{
    QList<ToolingRecommendationResult> results;
    errorMessage.clear();

    if (!m_toolingRepository) {
        errorMessage = "Takım deposu kullanılamıyor.";
        return results;
    }

    const QList<ToolingRecord> toolingList = m_toolingRepository->getAllTooling(errorMessage);
    if (!errorMessage.isEmpty())
        return results;

    const double targetVdie = thickness * 8.0;

    for (const ToolingRecord &tooling : toolingList) {
        bool vOk = false;
        QLocale::c().toDouble(tooling.vDieMm.trimmed(), &vOk);
        if (!vOk)
            continue;

        ToolingRecommendationResult result;
        result.tooling = tooling;
        result.reason = "Hedef V kanalına en yakın değer ≈ " +
                        formatRecommendationNumber(QString::number(targetVdie)) + " mm.";
        results.append(result);
    }

    std::sort(results.begin(), results.end(),
              [targetVdie](const ToolingRecommendationResult &a,
                           const ToolingRecommendationResult &b) {
                  bool okA = false;
                  bool okB = false;

                  const double aV = QLocale::c().toDouble(a.tooling.vDieMm, &okA);
                  const double bV = QLocale::c().toDouble(b.tooling.vDieMm, &okB);

                  const double aDiff = okA ? std::abs(aV - targetVdie) : 1e12;
                  const double bDiff = okB ? std::abs(bV - targetVdie) : 1e12;

                  return aDiff < bDiff;
              });

    if (results.size() > 5)
        results = results.mid(0, 5);

    return results;
}