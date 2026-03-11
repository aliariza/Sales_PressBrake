#ifndef STYLEHELPER_H
#define STYLEHELPER_H

class QPushButton;

namespace StyleHelper
{
void makeButtonClickable(QPushButton *button);
void applyPrimaryButtonStyle(QPushButton *button);
void applySecondaryButtonStyle(QPushButton *button);
}

#endif // STYLEHELPER_H
