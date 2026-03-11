#ifndef MACHINESTABLEWIDGET_H
#define MACHINESTABLEWIDGET_H

#include <QWidget>
#include <QList>
#include "machinerecord.h"

class QTableWidget;

class MachinesTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MachinesTableWidget(QWidget *parent = nullptr);

    void setMachines(const QList<MachineRecord> &machines);

signals:
    void editRequested(const MachineRecord &machine);
    void deleteRequested(const MachineRecord &machine);

private:
    void setupUi();

    QTableWidget *m_table;
};

#endif // MACHINESTABLEWIDGET_H
