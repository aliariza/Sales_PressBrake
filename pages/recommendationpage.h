#ifndef RECOMMENDATIONPAGE_H
#define RECOMMENDATIONPAGE_H

#include <QWidget>
#include <QList>
#include "recommendationresult.h"
#include "toolingrecommendationresult.h"
#include "materialrepository.h"
#include "machinerepository.h"
#include "toolingrepository.h"
#include "optionrepository.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QLabel;
class QTableWidget;

class RecommendationPage : public QWidget
{
    Q_OBJECT

public:
    explicit RecommendationPage(QWidget *parent = nullptr);

    void refreshData();

signals:
    void logoutRequested();

private slots:
    void runRecommendation();
    void updateQuoteSummary();

private:
    void setupUi();
    void loadMaterials();
    void loadOptions();
    void showMachineResults(const QList<RecommendationResult> &results);
    void showToolingResults(const QList<ToolingRecommendationResult> &results);
    void populateSelectionCombos();
    double selectedMachinePrice() const;
    double selectedOptionsTotal() const;
    QString currentMachinePriceText() const;

    QList<RecommendationResult> buildRecommendations(const QString &materialName,
                                                     double thickness,
                                                     double bendLength,
                                                     QString &errorMessage) const;

    QList<ToolingRecommendationResult> buildToolingRecommendations(double thickness,
                                                                   QString &errorMessage) const;

    QPushButton *m_logoutButton;
    QComboBox *m_materialComboBox;
    QLineEdit *m_thicknessEdit;
    QLineEdit *m_bendLengthEdit;
    QPushButton *m_recommendButton;
    QLabel *m_statusLabel;
    QTableWidget *m_resultsTable;
    QTableWidget *m_toolingResultsTable;

    QComboBox *m_selectedMachineComboBox;
    QComboBox *m_selectedToolingComboBox;
    QLabel *m_machinePriceLabel;
    QLabel *m_toolingInfoLabel;
    QLabel *m_optionsTotalLabel;
    QLabel *m_grandTotalLabel;

    QTableWidget *m_optionsTable;

    QList<RecommendationResult> m_currentMachineResults;
    QList<ToolingRecommendationResult> m_currentToolingResults;
    QList<OptionRecord> m_allOptions;

    MaterialRepository m_materialRepository;
    MachineRepository m_machineRepository;
    ToolingRepository m_toolingRepository;
    OptionRepository m_optionRepository;
};

#endif // RECOMMENDATIONPAGE_H
