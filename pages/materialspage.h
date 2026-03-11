#ifndef MATERIALSPAGE_H
#define MATERIALSPAGE_H

#include <QWidget>
#include "materialrepository.h"

class QPushButton;
class MaterialFormWidget;
class MaterialsTableWidget;

class MaterialsPage : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialsPage(QWidget *parent = nullptr);

    void refreshMaterialsTable();

signals:


private slots:
    void createOrUpdateMaterial();
    void cancelEdit();
    void handleEditRequested(const MaterialRecord &material);
    void handleDeleteRequested(const MaterialRecord &material);

private:
    void setupUi();
    void enterEditMode(const MaterialRecord &material);
    void exitEditMode();


    MaterialFormWidget *m_formWidget;
    MaterialsTableWidget *m_tableWidget;

    bool m_isEditMode;
    QString m_editingNo;

    MaterialRepository m_materialRepository;
};

#endif // MATERIALSPAGE_H
