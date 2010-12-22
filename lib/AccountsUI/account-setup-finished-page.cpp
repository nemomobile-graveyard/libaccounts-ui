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

//libAccounts
#include <Accounts/Provider>
#include <Accounts/Account>
#include <Accounts/ServiceType>
#include <Accounts/Manager>

//Meegotouch
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MContentItem>
#include <MLocale>
#include <MAction>
#include <MLabel>
#include <MGridLayoutPolicy>
#include <MSeparator>
#include <MImageWidget>
#include <MButton>

//Qt
#include <QDebug>

//Accounts-Ui
#include "account-setup-finished-page-priv.h"
#include "accountsmanagersingleton.h"
#include "provider-plugin-process.h"

namespace AccountsUI {

AccountSetupFinishedPage::AccountSetupFinishedPage(AbstractAccountSetupContext *context)
        : MApplicationPage(),
          d_ptr(new AccountSetupFinishedPagePrivate())
{
    Q_D(AccountSetupFinishedPage);
    d->q_ptr = this;
    d->account = context->account();
    d->serviceType = context->serviceType();
    setObjectName("AccountSetupFinishedPage");
}

AccountSetupFinishedPage::~AccountSetupFinishedPage()
{
    delete d_ptr;
}

void AccountSetupFinishedPage::createContent()
{
    Q_D(AccountSetupFinishedPage);

    setComponentsDisplayMode(EscapeButton, MApplicationPageModel::Hide);

    MWidget *centralWidget = new MWidget();
    MLayout *layout = new MLayout(centralWidget);
    MLinearLayoutPolicy *layoutPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    layoutPolicy->setSpacing(0);

    QString providerName(d->account->providerName());
    // xml file that describes the ui elements for the provider
    Accounts::Provider *provider = AccountsManager::instance()->provider(providerName);
    if (provider) {
        QDomElement root = provider->domDocument().documentElement();
        QDomElement providerIcon = root.firstChildElement("icon");
        QString providerIconId = providerIcon.text();

        MContentItem *providerItem = new MContentItem(MContentItem::IconAndSingleTextLabel, this);
        providerItem->setStyleName("SetupFinishedProviderName");
        providerItem->setImageID(providerIconId);
        providerItem->setTitle(provider->displayName());
        layoutPolicy->addItem(providerItem);
    }

    MSeparator *separatorTop = new MSeparator(this);
    separatorTop->setOrientation(Qt::Horizontal);

    layoutPolicy->addItem(separatorTop);

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

    layoutPolicy->addItem(successLabel, Qt::AlignCenter);
    layoutPolicy->addItem(firstDescLabel, Qt::AlignCenter);
    layoutPolicy->addItem(secondDescLabel, Qt::AlignCenter);

    Accounts::ServiceType *type = AccountsManager::instance()->serviceType(d->serviceType);

    //% "Go to %1"
    MButton *goToButton = new MButton(qtTrId("qtn_comm_go_to_x").arg(type->displayName()));

    connect (goToButton, SIGNAL(clicked()), this, SLOT(goToApplication()));
    //% "Add more account"
    MButton *addMoreAccountButton = new MButton(qtTrId("qtn_acc_add_more_accounts"));
    connect (addMoreAccountButton, SIGNAL(clicked()), ProviderPluginProcess::instance(), SLOT(quit()));
    MLayout *buttonsLayout = new MLayout();
    MLinearLayoutPolicy *portraitPolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Vertical);
    MLinearLayoutPolicy *landscapePolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Horizontal);

    portraitPolicy->addStretch();
    portraitPolicy->setSpacing(20);
    landscapePolicy->setSpacing(20);

    portraitPolicy->addItem(goToButton, Qt::AlignCenter);
    portraitPolicy->addStretch();
    portraitPolicy->addItem(addMoreAccountButton, Qt::AlignCenter);
    landscapePolicy->addStretch();
    landscapePolicy->addItem(goToButton, Qt::AlignRight);

    landscapePolicy->addItem(addMoreAccountButton, Qt::AlignCenter);
    landscapePolicy->addStretch();

    buttonsLayout->setLandscapePolicy(landscapePolicy);
    buttonsLayout->setPortraitPolicy(portraitPolicy);
    layoutPolicy->addStretch();
    layoutPolicy->addItem(buttonsLayout);

    setCentralWidget(centralWidget);

}

void AccountSetupFinishedPage::goToApplication()
{
    ProviderPluginProcess::instance()->setReturnToApp(true);
    ProviderPluginProcess::instance()->quit();
}

}
