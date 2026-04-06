#ifndef TOASTMESSAGE_H
#define TOASTMESSAGE_H

#include <QFrame>

class QLabel;
class QTimer;
class QEvent;

class ToastMessage : public QFrame
{
    Q_OBJECT

public:
    explicit ToastMessage(QWidget *parent = nullptr);

    void showMessage(const QString &text,
                     const QString &bgColor,
                     const QString &textColor = "#ffffff",
                     int timeoutMs = 3000);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void reposition();

    QLabel *m_label;
    QTimer *m_timer;
};

#endif // TOASTMESSAGE_H