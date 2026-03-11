#ifndef TOOLINGFORMWIDGET_H
#define TOOLINGFORMWIDGET_H

#include <QWidget>
#include "toolingrecord.h"

class QLineEdit;
class QPushButton;
class QLabel;

class ToolingFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolingFormWidget(QWidget *parent = nullptr);

    QString name() const;
    QString vDieMm() const;
    QString punchRadiusMm() const;
    QString dieRadiusMm() const;

    bool hasAcceptableNumericInputs() const;

    void setCreateMode();
    void setEditMode(const ToolingRecord &tooling);

    void clearForm();
    void setStatusMessage(const QString &text);
    void clearStatusMessage();
    void focusName();

signals:
    void submitRequested();
    void cancelEditRequested();

private:
    void setupUi();

    QLineEdit *m_nameEdit;
    QLineEdit *m_vDieEdit;
    QLineEdit *m_punchRadiusEdit;
    QLineEdit *m_dieRadiusEdit;

    QPushButton *m_submitButton;
    QPushButton *m_cancelEditButton;

    QLabel *m_statusLabel;
};

#endif // TOOLINGFORMWIDGET_H
