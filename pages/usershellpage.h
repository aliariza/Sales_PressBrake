#ifndef USERSHELLPAGE_H
#define USERSHELLPAGE_H

#include <QWidget>

class QPushButton;
class QStackedWidget;
class RecommendationPage;
class QuotesPage;

class UserShellPage : public QWidget
{
    Q_OBJECT

public:
    explicit UserShellPage(QWidget *parent = nullptr);

    void showRecommendationPage();
    void showQuotesPage();

signals:
    void logoutRequested();

private:
    void setupUi();
    void updateNavStyles(QPushButton *activeButton);

    QPushButton *m_recommendButton;
    QPushButton *m_quotesButton;
    QPushButton *m_logoutButton;

    QStackedWidget *m_contentStack;

    RecommendationPage *m_recommendationPage;
    QuotesPage *m_quotesPage;
};

#endif // USERSHELLPAGE_H
