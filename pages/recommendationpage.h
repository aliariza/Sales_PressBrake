#ifndef RECOMMENDATIONPAGE_H
#define RECOMMENDATIONPAGE_H

#include <QList>
#include <QWidget>

#include "materialrepository.h"
#include "machinerepository.h"
#include "optionrepository.h"
#include "quoterepository.h"
#include "recommendationresult.h"
#include "recommendationservice.h"
#include "toolingrecommendationresult.h"
#include "toolingrepository.h"

class QComboBox;
class QFont;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QTextEdit;
class ToastMessage;

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
    void saveQuote();

private:
    void setupUi();
    void loadMaterials();
    void loadOptions();

    QGroupBox *createInputGroup(const QFont &sectionFont);
    QGroupBox *createMachinesGroup(const QFont &sectionFont);
    QGroupBox *createToolingGroup(const QFont &sectionFont);
    QGroupBox *createOptionsGroup(const QFont &sectionFont);
    QGroupBox *createQuoteGroup(const QFont &sectionFont);
    void connectSignals();

    void selectDefaultRows();

    int selectedMachineRow() const;
    int selectedToolingRow() const;

    void showSuccessToast(const QString &message);
    void showErrorToast(const QString &message);
    void showInfoToast(const QString &message);

    double selectedMachinePrice() const;
    double selectedOptionsTotal() const;
    QString buildSelectedOptionsData() const;

    QComboBox *m_materialComboBox;
    QLineEdit *m_thicknessEdit;
    QLineEdit *m_bendLengthEdit;
    QTextEdit *m_notesEdit;
    QPushButton *m_recommendButton;
    QPushButton *m_saveQuoteButton;

    QTableWidget *m_resultsTable;
    QTableWidget *m_toolingResultsTable;
    QTableWidget *m_optionsTable;

    QLabel *m_selectedMachineValueLabel;
    QLabel *m_selectedToolingValueLabel;
    QLabel *m_machinePriceLabel;
    QLabel *m_toolingInfoLabel;
    QLabel *m_optionsTotalLabel;
    QLabel *m_grandTotalLabel;

    ToastMessage *m_toast;

    QList<RecommendationResult> m_currentMachineResults;
    QList<ToolingRecommendationResult> m_currentToolingResults;
    QList<OptionRecord> m_allOptions;

    MaterialRepository m_materialRepository;
    MachineRepository m_machineRepository;
    ToolingRepository m_toolingRepository;
    OptionRepository m_optionRepository;
    QuoteRepository m_quoteRepository;
    RecommendationService m_recommendationService;
};

#endif // RECOMMENDATIONPAGE_H