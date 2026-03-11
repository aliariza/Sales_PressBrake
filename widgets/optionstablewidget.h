#ifndef OPTIONSTABLEWIDGET_H
#define OPTIONSTABLEWIDGET_H

#include <QWidget>
#include <QList>
#include "optionrecord.h"

class QTableWidget;

class OptionsTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsTableWidget(QWidget *parent = nullptr);

    void setOptions(const QList<OptionRecord> &options);

signals:
    void editRequested(const OptionRecord &option);
    void deleteRequested(const OptionRecord &option);

private:
    void setupUi();

    QTableWidget *m_table;
};

#endif // OPTIONSTABLEWIDGET_H
