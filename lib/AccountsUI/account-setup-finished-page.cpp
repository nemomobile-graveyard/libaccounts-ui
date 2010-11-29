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
#include "account-setup-finished-page.h"
#include "accountsmanagersingleton.h"
#include "provider-plugin-process.h"

namespace AccountsUI {

static AccountSetupFinishedPage *plugin_instance = 0;

class AccountSetupFinishedPagePrivate
{
public:
    AccountSetupFinishedPagePrivate()
            : account(0)
    {}

    ~AccountSetupFinishedPagePrivate() {}
    Accounts::Account *account;
    QString serviceType;
};

AccountSetupFinishedPage::AccountSetupFinishedPage(Accounts::Account *account, QString serviceType)
        : MApplicationPage(),
          d_ptr(new AccountSetupFinishedPagePrivate())
{
    Q_D(AccountSetupFinishedPage);
    d->account = account;
    d->serviceType = serviceType;
    setObjectName("AccountSetupFinishedPage");
//    if (plugin_instance != 0)
//        qWarning() << "ProviderPluginProcess already instantiated";

}

AccountSetupFinishedPage::~AccountSetupFinishedPage()
{
    delete d_ptr;
}

//AccountSetupFinishedPage *AccountSetupFinishedPage::instance()
//{
//    return plugin_instance;
//}

void AccountSetupFinishedPage::createContent()
{
    Q_D(AccountSetupFinishedPage);
//    plugin_instance = this;
    MWidget* centralWidget = new MWidget();
    MLayout* layout = new MLayout(centralWidget);
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

    // %"Your %1 account is connected!"
    MLabel *firstDescLabel = new MLabel(qtTrId("qtn_acc_account_connected").arg(providerName));
    firstDescLabel->setStyleName("FirstDescLabel");

    // %"Fetching your data"
    MLabel *secondDescLabel = new MLabel(qtTrId("qtn_acc_fetching_your_data"));
    secondDescLabel->setStyleName("SecondDescLabel");

    layoutPolicy->addItem(successLabel, Qt::AlignCenter);
    layoutPolicy->addItem(firstDescLabel, Qt::AlignCenter);
    layoutPolicy->addItem(secondDescLabel, Qt::AlignCenter);

    Accounts::ServiceType *type = AccountsManager::instance()->serviceType(d->serviceType);

    //% "Go to %1"
    MButton *goToButton = new MButton(qtTrId("qtn_comm_go_to_x").arg(type->displayName()));

    connect (goToButton, SIGNAL(clicked()), this, SLOT(goToApplication()));
    connect (goToButton, SIGNAL(clicked()), ProviderPluginProcess::instance(), SLOT(quit()));
    //% "Add more account"
    MButton *addMoreAccountButton = new MButton(qtTrId("qtn_acc_add_more_accounts"));
    connect (addMoreAccountButton, SIGNAL(clicked()), ProviderPluginProcess::instance(), SLOT(quit()));
    MLayout *buttonsLayout = new MLayout();
    MLinearLayoutPolicy *portraitPolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Vertical);
    MLinearLayoutPolicy *landscapePolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Horizontal);

    portraitPolicy->addStretch();
    landscapePolicy->addStretch();
    portraitPolicy->setSpacing(20);
    landscapePolicy->setSpacing(20);

    portraitPolicy->addItem(goToButton, Qt::AlignCenter);
    portraitPolicy->addItem(addMoreAccountButton, Qt::AlignCenter);
    landscapePolicy->addItem(goToButton, Qt::AlignCenter);
    landscapePolicy->addItem(addMoreAccountButton, Qt::AlignCenter);

    buttonsLayout->setLandscapePolicy(landscapePolicy);
    buttonsLayout->setPortraitPolicy(portraitPolicy);

    layoutPolicy->addItem(buttonsLayout);

    setCentralWidget(centralWidget);

}

void AccountSetupFinishedPage::goToApplication()
{
    system("echo okkkkkkkkkkkkkkkkkk");
    ProviderPluginProcess::instance()->emitQuitMainApp();
}

void AccountSetupFinishedPage::addMoreAccount()
{}
}
