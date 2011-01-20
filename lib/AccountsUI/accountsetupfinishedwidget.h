#ifndef ACCOUNTSETUPFINISHEDWIDGET_H
#define ACCOUNTSETUPFINISHEDWIDGET_H

#include <MStylableWidget>

class AccountSetupFinishedWidget : public MStylableWidget
{
    Q_OBJECT

public:
    AccountSetupFinishedWidget(const QString &providerName, QGraphicsItem *parent = 0);
};

#endif // ACCOUNTSETUPFINISHEDWIDGET_H
