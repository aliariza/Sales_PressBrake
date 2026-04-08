#include "usershellpage.h"
#include "recommendationpage.h"
#include "quotespage.h"
#include "stylehelper.h"

#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QFont>
#include <QSizePolicy>
#include <QTimer>

UserShellPage::UserShellPage(QWidget *parent)
    : QWidget(parent),
    m_recommendButton(nullptr),
    m_quotesButton(nullptr),
    m_logoutButton(nullptr),
    m_contentStack(nullptr),
    m_recommendationPage(nullptr),
    m_quotesPage(nullptr)
{
    setupUi();
    showRecommendationPage();

    QTimer::singleShot(0, this, [this]() {
        updateNavStyles(m_recommendButton);
    });
}

static bool isDarkTheme(const QWidget *w)
{
    return w->palette().color(QPalette::Window).lightness() < 128;
}

void UserShellPage::setupUi()
{
    auto *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *sidebarFrame = new QFrame(this);
    sidebarFrame->setFixedWidth(220);
    sidebarFrame->setObjectName("userSidebar");

    if (isDarkTheme(this)) {
        sidebarFrame->setStyleSheet(
            "#userSidebar {"
            " border-right: 1px solid #4a4a4a;"
            " background-color: #2b2b2b;"
            " }"
            );
    } else {
        sidebarFrame->setStyleSheet(
            "#userSidebar {"
            " border-right: 1px solid #d8d8d8;"
            " background-color: #f7f7f7;"
            " }"
            );
    }

    auto *sidebarLayout = new QVBoxLayout(sidebarFrame);
    sidebarLayout->setContentsMargins(10, 18, 10, 18);
    sidebarLayout->setSpacing(12);

    auto *titleLabel = new QLabel("KULLANICI", sidebarFrame);
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    if (isDarkTheme(this)) {
        titleLabel->setStyleSheet("color: #F2F2F2; background: transparent;");
    } else {
        titleLabel->setStyleSheet("color: #111111; background: transparent;");
    }

    m_recommendButton = new QPushButton("ÖNERİ", sidebarFrame);
    m_quotesButton = new QPushButton("TEKLİFLER", sidebarFrame);
    m_logoutButton = new QPushButton("ÇIKIŞ", sidebarFrame);

    const QList<QPushButton*> navButtons{
        m_recommendButton, m_quotesButton
    };

    for (QPushButton *button : navButtons) {
        button->setFixedHeight(42);
        button->setMinimumWidth(0);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        button->setCursor(Qt::PointingHandCursor);
        StyleHelper::applySecondaryButtonStyle(button);
    }

    m_logoutButton->setFixedHeight(42);
    m_logoutButton->setMinimumWidth(0);
    m_logoutButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_logoutButton->setCursor(Qt::PointingHandCursor);
    StyleHelper::applySecondaryButtonStyle(m_logoutButton);

    sidebarLayout->addWidget(titleLabel);
    sidebarLayout->addSpacing(8);
    sidebarLayout->addWidget(m_recommendButton);
    sidebarLayout->addWidget(m_quotesButton);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(m_logoutButton);

    m_contentStack = new QStackedWidget(this);

    m_recommendationPage = new RecommendationPage(this);
    m_quotesPage = new QuotesPage(this);

    m_contentStack->addWidget(m_recommendationPage);
    m_contentStack->addWidget(m_quotesPage);

    rootLayout->addWidget(sidebarFrame);
    rootLayout->addWidget(m_contentStack, 1);

    connect(m_recommendButton, &QPushButton::clicked, this, &UserShellPage::showRecommendationPage);
    connect(m_quotesButton, &QPushButton::clicked, this, &UserShellPage::showQuotesPage);
    connect(m_logoutButton, &QPushButton::clicked, this, &UserShellPage::logoutRequested);

    connect(m_recommendationPage, &RecommendationPage::logoutRequested, this, &UserShellPage::logoutRequested);
}

void UserShellPage::updateNavStyles(QPushButton *activeButton)
{
    const QList<QPushButton*> navButtons{
        m_recommendButton, m_quotesButton
    };

    for (QPushButton *button : navButtons) {
        if (button == activeButton) {
            StyleHelper::applyPrimaryButtonStyle(button);
        } else {
            StyleHelper::applySecondaryButtonStyle(button);
        }
    }

    StyleHelper::applySecondaryButtonStyle(m_logoutButton);
}

void UserShellPage::showRecommendationPage()
{
    m_recommendationPage->refreshData();
    m_contentStack->setCurrentWidget(m_recommendationPage);
    updateNavStyles(m_recommendButton);
}

void UserShellPage::showQuotesPage()
{
    m_quotesPage->refreshQuotesTable();
    m_contentStack->setCurrentWidget(m_quotesPage);
    updateNavStyles(m_quotesButton);
}
