#ifndef OPTIONFORMWIDGET_H
#define OPTIONFORMWIDGET_H

#include <QWidget>
#include "optionrecord.h"

class QLineEdit;
class QPushButton;
class QLabel;

class OptionFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OptionFormWidget(QWidget *parent = nullptr);

    QString code() const;
    QString name() const;
    QString priceUsd() const;

    bool hasAcceptableNumericInputs() const;

    void setCreateMode();
    void setEditMode(const OptionRecord &option);

    void clearForm();
    void setStatusMessage(const QString &text);
    void clearStatusMessage();
    void focusCode();

signals:
    void submitRequested();
    void cancelEditRequested();

private:
    void setupUi();

    QLineEdit *m_codeEdit;
    QLineEdit *m_nameEdit;
    QLineEdit *m_priceUsdEdit;

    QPushButton *m_submitButton;
    QPushButton *m_cancelEditButton;

    QLabel *m_statusLabel;
};

#endif // OPTIONFORMWIDGET_H
