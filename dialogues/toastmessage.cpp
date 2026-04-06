#include "toastmessage.h"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QEvent>
#include <QWidget>

ToastMessage::ToastMessage(QWidget *parent)
    : QFrame(parent),
    m_label(new QLabel(this)),
    m_timer(new QTimer(this))
{
    setVisible(false);
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setObjectName("toastMessage");

    // Important:
    // Do NOT use Qt::ToolTip if you want this centered inside the parent widget.
    setWindowFlags(Qt::FramelessWindowHint);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->addWidget(m_label);

    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setMinimumWidth(360);
    m_label->setMaximumWidth(520);

    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        hide();
    });

    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
    }
}

void ToastMessage::showMessage(const QString &text,
                               const QString &bgColor,
                               const QString &textColor,
                               int timeoutMs)
{
    m_label->setText(text);

    setStyleSheet(
        QString(
            "QFrame#toastMessage {"
            " background-color: %1;"
            " border: 1px solid rgba(255,255,255,0.12);"
            " border-radius: 12px;"
            " }"
            "QLabel {"
            " color: %2;"
            " font-weight: 600;"
            " padding: 6px 10px;"
            " }"
            ).arg(bgColor, textColor)
        );

    adjustSize();
    resize(sizeHint());
    reposition();

    show();
    raise();

    m_timer->start(timeoutMs);
}

bool ToastMessage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && isVisible()) {
        if (event->type() == QEvent::Resize ||
            event->type() == QEvent::Move ||
            event->type() == QEvent::Show) {
            reposition();
        }
    }

    return QFrame::eventFilter(watched, event);
}

void ToastMessage::reposition()
{
    if (!parentWidget())
        return;

    const int x = (parentWidget()->width() - width()) / 2;
    const int y = (parentWidget()->height() - height()) / 2;

    move(x, y);
}