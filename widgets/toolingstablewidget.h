#ifndef TOOLINGSTABLEWIDGET_H
#define TOOLINGSTABLEWIDGET_H

#include <QWidget>
#include <QList>
#include "toolingrecord.h"

class QTableWidget;

class ToolingsTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolingsTableWidget(QWidget *parent = nullptr);

    void setToolingList(const QList<ToolingRecord> &toolingList);

signals:
    void editRequested(const ToolingRecord &tooling);
    void deleteRequested(const ToolingRecord &tooling);

private:
    void setupUi();

    QTableWidget *m_table;
};

#endif // TOOLINGSTABLEWIDGET_H
