#include "recommendationtablehelper.h"
#include "recommendationformatutils.h"

#include <QTableWidget>
#include <QTableWidgetItem>

void RecommendationTableHelper::fillMachineResultsTable(QTableWidget *table,
                                                        const QList<RecommendationResult> &results)
{
    if (!table)
        return;

    table->setRowCount(0);

    for (int i = 0; i < results.size(); ++i) {
        const RecommendationResult &r = results[i];
        table->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, col, item);
        };

        set(0, "Abkant Pres");
        set(1, r.machine.model);
        set(2, formatRecommendationNumber(r.machine.maxTonnageTonf), true);
        set(3, formatRecommendationNumber(r.machine.workingLengthMm, true), true);
        set(4, formatRecommendationNumber(r.machine.maxThicknessMm), true);
        set(5, formatRecommendationNumber(r.machine.basePriceUSD, true), true);
        set(6, r.reason);
    }
}

void RecommendationTableHelper::fillToolingResultsTable(QTableWidget *table,
                                                        const QList<ToolingRecommendationResult> &results)
{
    if (!table)
        return;

    table->setRowCount(0);

    for (int i = 0; i < results.size(); ++i) {
        const ToolingRecommendationResult &r = results[i];
        table->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, col, item);
        };

        set(0, r.tooling.name);
        set(1, formatRecommendationNumber(r.tooling.vDieMm), true);
        set(2, formatRecommendationNumber(r.tooling.punchRadiusMm), true);
        set(3, formatRecommendationNumber(r.tooling.dieRadiusMm), true);
        set(4, r.reason);
    }
}

void RecommendationTableHelper::fillOptionsTable(QTableWidget *table,
                                                 const QList<OptionRecord> &options)
{
    if (!table)
        return;

    table->setRowCount(0);

    for (int i = 0; i < options.size(); ++i) {
        const OptionRecord &option = options[i];
        table->insertRow(i);

        auto *checkItem = new QTableWidgetItem;
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        checkItem->setCheckState(Qt::Unchecked);

        auto *codeItem = new QTableWidgetItem(option.code);
        auto *nameItem = new QTableWidgetItem(option.name);
        auto *priceItem = new QTableWidgetItem(formatRecommendationNumber(option.priceUsd, true));

        codeItem->setTextAlignment(Qt::AlignCenter);
        priceItem->setTextAlignment(Qt::AlignCenter);

        table->setItem(i, 0, checkItem);
        table->setItem(i, 1, codeItem);
        table->setItem(i, 2, nameItem);
        table->setItem(i, 3, priceItem);
    }
}