#include "welcomepage.h"
#include "shortcuthelper.h"
#include "stylehelper.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent),
    titleLabel(nullptr),
    welcomeLoginButton(nullptr),
    welcomeCancelButton(nullptr)
{
    setupUi();
}

void WelcomePage::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(24, 24, 24, 24);

    auto *container = new QWidget(this);
    container->setMaximumWidth(520);

    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(24, 24, 24, 24);
    containerLayout->setSpacing(18);

    titleLabel = new QLabel("Welcome", container);
    titleLabel->setAlignment(Qt::AlignCenter);

    QFont titleFont;
    titleFont.setPointSize(26);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    auto *subtitleLabel = new QLabel("Please choose an action to continue", container);
    subtitleLabel->setAlignment(Qt::AlignCenter);

    QFont subtitleFont;
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);

    welcomeLoginButton = new QPushButton("LOGIN", container);
    welcomeCancelButton = new QPushButton("CANCEL", container);

    welcomeLoginButton->setFixedSize(180, 42);
    welcomeCancelButton->setFixedSize(180, 42);

    welcomeLoginButton->setDefault(true);
    welcomeLoginButton->setAutoDefault(true);
    welcomeLoginButton->setFocus();

    StyleHelper::applyPrimaryButtonStyle(welcomeLoginButton);
    StyleHelper::applySecondaryButtonStyle(welcomeCancelButton);
    StyleHelper::makeButtonClickable(welcomeLoginButton);
    StyleHelper::makeButtonClickable(welcomeCancelButton);


    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(16);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->addWidget(welcomeLoginButton);
    buttonLayout->addWidget(welcomeCancelButton);

    containerLayout->addWidget(titleLabel);
    containerLayout->addWidget(subtitleLabel);
    containerLayout->addSpacing(8);
    containerLayout->addLayout(buttonLayout);

    rootLayout->addStretch();

    auto *centerLayout = new QHBoxLayout;
    centerLayout->addStretch();
    centerLayout->addWidget(container);
    centerLayout->addStretch();

    rootLayout->addLayout(centerLayout);
    rootLayout->addStretch();

    connect(welcomeLoginButton, &QPushButton::clicked, this, &WelcomePage::loginRequested);
    connect(welcomeCancelButton, &QPushButton::clicked, this, &WelcomePage::cancelRequested);

    ShortcutHelper::bindEnterKeys(this, this, &WelcomePage::loginRequested);
    ShortcutHelper::bindEscapeKey(this, this, &WelcomePage::cancelRequested);

    container->setObjectName("welcomeCard");
    container->setStyleSheet(
        "#welcomeCard {"
        " border: 1px solid #d8d8d8;"
        " border-radius: 10px;"
        " }"
        );
}
void WelcomePage::focusLoginButton()
{
    if (welcomeLoginButton)
        welcomeLoginButton->setFocus();
}
