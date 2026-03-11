#ifndef TOOLINGPAGE_H
#define TOOLINGPAGE_H

#include <QWidget>
#include "toolingrepository.h"

class QPushButton;
class ToolingFormWidget;
class ToolingsTableWidget;

class ToolingPage : public QWidget
{
    Q_OBJECT

public:
    explicit ToolingPage(QWidget *parent = nullptr);

    void refreshToolingTable();

signals:


private slots:
    void createOrUpdateTooling();
    void cancelEdit();
    void handleEditRequested(const ToolingRecord &tooling);
    void handleDeleteRequested(const ToolingRecord &tooling);

private:
    void setupUi();
    void enterEditMode(const ToolingRecord &tooling);
    void exitEditMode();


    ToolingFormWidget *m_formWidget;
    ToolingsTableWidget *m_tableWidget;

    bool m_isEditMode;
    QString m_editingNo;

    ToolingRepository m_toolingRepository;
};

#endif // TOOLINGPAGE_H
