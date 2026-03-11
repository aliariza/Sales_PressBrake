#ifndef SHORTCUTHELPER_H
#define SHORTCUTHELPER_H

#include <QShortcut>
#include <QKeySequence>
#include <QWidget>
#include <QObject>

namespace ShortcutHelper
{
template<typename Receiver, typename Func>
void bindEnterKeys(QWidget *parent, Receiver *receiver, Func slot)
{
    auto *returnShortcut = new QShortcut(QKeySequence(Qt::Key_Return), parent);
    QObject::connect(returnShortcut, &QShortcut::activated, receiver, slot);

    auto *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Enter), parent);
    QObject::connect(enterShortcut, &QShortcut::activated, receiver, slot);
}

template<typename Receiver, typename Func>
void bindEscapeKey(QWidget *parent, Receiver *receiver, Func slot)
{
    auto *escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), parent);
    QObject::connect(escapeShortcut, &QShortcut::activated, receiver, slot);
}
}

#endif // SHORTCUTHELPER_H
