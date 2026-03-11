#ifndef OPTIONSPAGE_H
#define OPTIONSPAGE_H

#include <QWidget>
#include "optionrepository.h"

class OptionFormWidget;
class OptionsTableWidget;

class OptionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsPage(QWidget *parent = nullptr);

    void refreshOptionsTable();

private slots:
    void createOrUpdateOption();
    void cancelEdit();
    void handleEditRequested(const OptionRecord &option);
    void handleDeleteRequested(const OptionRecord &option);

private:
    void setupUi();
    void enterEditMode(const OptionRecord &option);
    void exitEditMode();

    OptionFormWidget *m_formWidget;
    OptionsTableWidget *m_tableWidget;

    bool m_isEditMode;
    QString m_editingNo;

    OptionRepository m_optionRepository;
};

#endif // OPTIONSPAGE_H
