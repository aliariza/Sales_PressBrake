#include "stylehelper.h"

#include <QPushButton>
#include <Qt>

namespace StyleHelper
{
void makeButtonClickable(QPushButton *button)
{
    if (!button)
        return;

    button->setCursor(Qt::PointingHandCursor);
}

void applyPrimaryButtonStyle(QPushButton *button)
{
    if (!button)
        return;

    button->setCursor(Qt::PointingHandCursor);

    button->setStyleSheet(
        "QPushButton {"
        " background-color: #0A84FF;"
        " color: white;"
        " border: 1px solid #0A84FF;"
        " border-radius: 8px;"
        " padding: 6px 10px;"
        " font-weight: 600;"
        "}"
        "QPushButton:hover {"
        " background-color: #339CFF;"
        "}"
        "QPushButton:pressed {"
        " background-color: #006EDC;"
        "}"
        "QPushButton:focus {"
        " border: 2px solid #66B2FF;"
        "}"
        );
}

void applySecondaryButtonStyle(QPushButton *button)
{
    if (!button)
        return;

    button->setCursor(Qt::PointingHandCursor);

    button->setStyleSheet(
        "QPushButton {"
        " background-color: #FFFFFF;"
        " color: black;"
        " border: 1px solid #0A84FF;"
        " border-radius: 8px;"
        " padding: 6px 10px;"
        " font-weight: 600;"
        "}"
        "QPushButton:hover {"
        " background-color: #0A84FF;"
        " color: white;"
        "}"
        "QPushButton:pressed {"
        " background-color: #006EDC;"
        " color: white;"
        "}"
        "QPushButton:focus {"
        " border: 2px solid #66B2FF;"
        "}"
        );
}
void applyDangerButtonStyle(QPushButton *button)
{
    if (!button)
        return;

    button->setCursor(Qt::PointingHandCursor);

    button->setStyleSheet(
        "QPushButton {"
        " background-color: #D32F2F;"
        " color: white;"
        " border: 1px solid #D32F2F;"
        " border-radius: 8px;"
        " padding: 6px 10px;"
        " font-weight: 600;"
        "}"
        "QPushButton:hover {"
        " background-color: #E53935;"
        "}"
        "QPushButton:pressed {"
        " background-color: #B71C1C;"
        "}"
        "QPushButton:focus {"
        " border: 2px solid #EF9A9A;"
        "}"
        );
}
}
