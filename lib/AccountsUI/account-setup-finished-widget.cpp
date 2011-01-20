/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "account-setup-finished-widget.h"

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


