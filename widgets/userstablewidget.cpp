#include "userstablewidget.h"
#include "stylehelper.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QFont>

UsersTableWidget::UsersTableWidget(QWidget *parent)
    : QWidget(parent),
    m_table(nullptr)
{
    setupUi();
}

void UsersTableWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *usersGroup = new QGroupBox("Existing Users", this);
    usersGroup->setFont(sectionFont);

    auto *usersLayout = new QVBoxLayout(usersGroup);
    usersLayout->setContentsMargins(16, 20, 16, 16);
    usersLayout->setSpacing(10);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels(
        QStringList() << "No" << "User" << "Role" << "Comments" << "Action"
        );

    auto *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents); // No
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents); // User
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Role
    header->setSectionResizeMode(3, QHeaderView::Stretch);          // Comments
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Action
    header->setDefaultAlignment(Qt::AlignCenter);

    m_table->horizontalHeaderItem(0)->setToolTip("Auto-generated user number");
    m_table->horizontalHeaderItem(1)->setToolTip("Username");
    m_table->horizontalHeaderItem(2)->setToolTip("User role");
    m_table->horizontalHeaderItem(3)->setToolTip("Notes or comments");
    m_table->horizontalHeaderItem(4)->setToolTip("Available actions");

    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setWordWrap(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(34);

    header->setStyleSheet(
        "QHeaderView::section {"
        " padding: 6px 8px;"
        " border: none;"
        " border-bottom: 1px solid #cfcfcf;"
        " font-weight: 600;"
        " }"
        );

    usersLayout->addWidget(m_table, 1);
    mainLayout->addWidget(usersGroup);
}

void UsersTableWidget::setUsers(const QList<User> &users)
{
    m_table->setRowCount(0);

    for (int i = 0; i < users.size(); ++i) {
        const User currentUser = users[i];

        m_table->insertRow(i);

        auto *noItem = new QTableWidgetItem(currentUser.userno);
        auto *userItem = new QTableWidgetItem(currentUser.username);
        auto *roleItem = new QTableWidgetItem(currentUser.role);
        auto *commentsItem = new QTableWidgetItem(currentUser.comments);

        noItem->setTextAlignment(Qt::AlignCenter);
        roleItem->setTextAlignment(Qt::AlignCenter);

        m_table->setItem(i, 0, noItem);
        m_table->setItem(i, 1, userItem);
        m_table->setItem(i, 2, roleItem);
        m_table->setItem(i, 3, commentsItem);

        auto *actionsWidget = new QWidget(m_table);
        auto *actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(4, 0, 4, 0);
        actionsLayout->setSpacing(6);

        auto *editButton = new QPushButton("Edit", actionsWidget);
        auto *deleteButton = new QPushButton("Delete", actionsWidget);

        editButton->setFixedHeight(24);
        deleteButton->setFixedHeight(24);
        editButton->setMinimumWidth(56);
        deleteButton->setMinimumWidth(64);

        StyleHelper::applySecondaryButtonStyle(editButton);
        StyleHelper::applySecondaryButtonStyle(deleteButton);

        connect(editButton, &QPushButton::clicked, this, [this, currentUser]() {
            emit editRequested(currentUser);
        });

        connect(deleteButton, &QPushButton::clicked, this, [this, currentUser]() {
            emit deleteRequested(currentUser);
        });

        actionsLayout->addWidget(editButton);
        actionsLayout->addWidget(deleteButton);

        m_table->setCellWidget(i, 4, actionsWidget);
    }
}
