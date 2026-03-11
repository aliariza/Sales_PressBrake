#ifndef MACHINEFORMWIDGET_H
#define MACHINEFORMWIDGET_H

#include <QWidget>
#include "machinerecord.h"

class QLineEdit;
class QPushButton;
class QLabel;

class MachineFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MachineFormWidget(QWidget *parent = nullptr);

    QString model() const;
    QString maxTonnageTonf() const;
    QString workingLengthMm() const;
    QString maxThicknessMm() const;
    QString basePriceUSD() const;

    bool hasAcceptableNumericInputs() const;

    void setCreateMode();
    void setEditMode(const MachineRecord &machine);

    void clearForm();
    void setStatusMessage(const QString &text);
    void clearStatusMessage();
    void focusModel();

signals:
    void submitRequested();
    void cancelEditRequested();

private:
    void setupUi();

    QLineEdit *m_modelEdit;
    QLineEdit *m_maxTonnageTonfEdit;
    QLineEdit *m_workingLengthMmEdit;
    QLineEdit *m_maxThicknessMmEdit;
    QLineEdit *m_basePriceUSDEdit;

    QPushButton *m_submitButton;
    QPushButton *m_cancelEditButton;

    QLabel *m_statusLabel;
};

#endif // MACHINEFORMWIDGET_H
