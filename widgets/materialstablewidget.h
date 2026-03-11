#ifndef MATERIALSTABLEWIDGET_H
#define MATERIALSTABLEWIDGET_H

#include <QWidget>
#include <QList>
#include "materialrecord.h"

class QTableWidget;

class MaterialsTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialsTableWidget(QWidget *parent = nullptr);

    void setMaterials(const QList<MaterialRecord> &materials);

signals:
    void editRequested(const MaterialRecord &material);
    void deleteRequested(const MaterialRecord &material);

private:
    void setupUi();

    QTableWidget *m_table;
};

#endif // MATERIALSTABLEWIDGET_H
