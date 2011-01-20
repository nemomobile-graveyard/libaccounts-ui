#include "accountsetupfinishedwidget.h"

#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MLabel>
#include <MLibrary>
#include <MWidgetCreator>
M_LIBRARY

M_REGISTER_WIDGET_NO_CREATE(AccountSetupFinishedWidget)

AccountSetupFinishedWidget::AccountSetupFinishedWidget(const QString &providerName,
                                                       QGraphicsItem *parent)
    :MStylableWidget(parent)
{
    MLayout *layout = new MLayout;
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);

    // %"Success"
    MLabel *successLabel = new MLabel(qtTrId("qtn_acc_success"));
    successLabel->setStyleName("SuccessLabel");
    successLabel->setAlignment(Qt::AlignCenter);

    // %"Your %1 account is connected!"
    MLabel *firstDescLabel = new MLabel(qtTrId("qtn_acc_account_connected").arg(providerName));
    firstDescLabel->setStyleName("FirstDescLabel");
    firstDescLabel->setAlignment(Qt::AlignCenter);

    // %"Fetching your data"
    MLabel *secondDescLabel = new MLabel(qtTrId("qtn_acc_fetching_your_data"));
    secondDescLabel->setStyleName("SecondDescLabel");
    secondDescLabel->setAlignment(Qt::AlignCenter);

    policy->addItem(successLabel, Qt::AlignCenter);
    policy->addItem(firstDescLabel, Qt::AlignCenter);
    policy->addItem(secondDescLabel, Qt::AlignCenter);
    setLayout(layout);
}


