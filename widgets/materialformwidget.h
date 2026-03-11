#ifndef MATERIALFORMWIDGET_H
#define MATERIALFORMWIDGET_H

#include <QWidget>
#include "materialrecord.h"

class QLineEdit;
class QPushButton;
class QLabel;

class MaterialFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialFormWidget(QWidget *parent = nullptr);

    QString name() const;
    QString tensileStrengthMPa() const;
    QString yieldStrengthMPa() const;
    QString kFactorDefault() const;
    QString youngsModulusMPa() const;
    QString recommendedVdieFactor() const;
    QString minThicknessMm() const;
    QString maxThicknessMm() const;

    bool hasAcceptableKFactor() const;
    bool hasAcceptableNumericInputs() const;

    void setCreateMode();
    void setEditMode(const MaterialRecord &material);

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
    QLineEdit *m_tensileStrengthEdit;
    QLineEdit *m_yieldStrengthEdit;
    QLineEdit *m_kFactorEdit;
    QLineEdit *m_youngsModulusEdit;
    QLineEdit *m_recommendedVdieFactorEdit;
    QLineEdit *m_minThicknessEdit;
    QLineEdit *m_maxThicknessEdit;

    QPushButton *m_submitButton;
    QPushButton *m_cancelEditButton;

    QLabel *m_statusLabel;
};

#endif // MATERIALFORMWIDGET_H
