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

#include "account-setup-finished-widget.h"
#include "accountsmanagersingleton.h"
#include "last-page-actions.h"
#include "provider-plugin-process.h"

#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MLabel>
#include <MLibrary>
#include <MWidgetCreator>
#include <MSeparator>
#include <MButton>
#include <MBasicListItem>
#include <MLocale>
#include <MImageWidget>

//Qt
#include <QDebug>

class AccountSetupFinishedListItem: public MBasicListItem
{
public:
    AccountSetupFinishedListItem(QGraphicsWidget *parent)
            : MBasicListItem(MBasicListItem::IconWithTitle, parent)
    {
        setStyleName("CommonBasicListItemInverted");
        titleLabelWidget()->setStyleName("AccountSetupFinishedLargeHeader");
        setEnabled(false);
    }

    ~AccountSetupFinishedListItem()
    {}
};

M_LIBRARY

M_REGISTER_WIDGET_NO_CREATE(AccountSetupFinishedWidget)

AccountSetupFinishedWidget::AccountSetupFinishedWidget(const QString &providerName,
                                                       QGraphicsItem *parent)
    :MStylableWidget(parent)
{
    MLayout *layout = new MLayout;
    MLinearLayoutPolicy *layoutPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);

    MSeparator *headerSpacer = new MSeparator(this);
    headerSpacer->setOrientation(Qt::Horizontal);
    headerSpacer->setStyleName("CommonSpacer");
    layoutPolicy->addItem(headerSpacer);

    // xml file that describes the ui elements for the provider
    Accounts::Provider *provider = AccountsUI::AccountsManager::instance()->provider(providerName);
    if (provider) {
        QDomElement root = provider->domDocument().documentElement();
        QDomElement providerIcon = root.firstChildElement("icon");
        QString catalog = provider->trCatalog();
        MLocale locale;
        if (!catalog.isEmpty() && !locale.isInstalledTrCatalog(catalog)) {
            locale.installTrCatalog(catalog);
            MLocale::setDefault(locale);
        }

        AccountSetupFinishedListItem *providerItem = new AccountSetupFinishedListItem(this);
        providerItem->imageWidget()->setImage(providerIcon.text());
        providerItem->setTitle(qtTrId(provider->displayName().toLatin1()));
        layoutPolicy->addItem(providerItem);
    }

    MSeparator *separatorTop = new MSeparator(this);
    separatorTop->setOrientation(Qt::Horizontal);
    separatorTop->setStyleName("CommonItemDividerInverted");

    layoutPolicy->addItem(separatorTop);
    layoutPolicy->addStretch();

    // %"Success"
    MLabel *successLabel = new MLabel(qtTrId("qtn_acc_success"));
    successLabel->setWordWrap(true);
    successLabel->setWrapMode(QTextOption::WordWrap);
    successLabel->setStyleName("SuccessLabel");
    successLabel->setAlignment(Qt::AlignCenter);

    // %"Your account is connected!"
    MLabel *firstDescLabel = new MLabel(qtTrId("qtn_acc_account_connected"));
    firstDescLabel->setWordWrap(true);
    firstDescLabel->setWrapMode(QTextOption::WordWrap);
    firstDescLabel->setStyleName("FirstDescLabel");
    firstDescLabel->setAlignment(Qt::AlignCenter);

    // %"Fetching your data"
    MLabel *secondDescLabel = new MLabel(qtTrId("qtn_acc_fetching_your_data"));
    secondDescLabel->setStyleName("SecondDescLabel");
    secondDescLabel->setWordWrap(true);
    secondDescLabel->setWrapMode(QTextOption::WordWrap);
    secondDescLabel->setAlignment(Qt::AlignCenter);

    layoutPolicy->addItem(successLabel, Qt::AlignCenter);
    layoutPolicy->addItem(firstDescLabel, Qt::AlignCenter);
    layoutPolicy->addItem(secondDescLabel, Qt::AlignCenter);
    layoutPolicy->addStretch();

    MSeparator *spacer = new MSeparator(this);
    spacer->setOrientation(Qt::Horizontal);
    spacer->setStyleName("AccountSetupFinishedPageXLargeSpacer");
    layoutPolicy->addItem(spacer);

    //% "Add more account"
    MButton *addMoreAccountButton = new MButton(qtTrId("qtn_acc_add_more_accounts"));
    addMoreAccountButton->setStyleName("CommonSingleButtonInverted");
    connect(addMoreAccountButton, SIGNAL(clicked()),
             AccountsUI::ProviderPluginProcess::instance(), SLOT(quit()));
    MLayout *buttonsLayout = new MLayout();
    MLinearLayoutPolicy *portraitPolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Vertical);
    MLinearLayoutPolicy *landscapePolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Horizontal);

    const AccountsUI::LastPageActions &lastPageActions =
            AccountsUI::ProviderPluginProcess::instance()->lastPageActions();
    const AccountsUI::LastPageActions::ServiceActionList actions =
            lastPageActions.serviceActions();
    foreach (AccountsUI::LastPageActions::ServiceAction action, actions) {
        MButton *button = new MButton(qtTrId("qtn_comm_go_to_x").
                                      arg(action.title()));
        button->setProperty("serviceName", action.serviceName());
        connect(button, SIGNAL(clicked()), this, SLOT(actionButtonClicked()));
        button->setStyleName("CommonSingleButtonInverted");

        landscapePolicy->addItem(button, Qt::AlignCenter);
        portraitPolicy->addItem(button, Qt::AlignCenter);
    }

    portraitPolicy->addItem(addMoreAccountButton, Qt::AlignCenter);
    landscapePolicy->addItem(addMoreAccountButton, Qt::AlignCenter);

    buttonsLayout->setLandscapePolicy(landscapePolicy);
    buttonsLayout->setPortraitPolicy(portraitPolicy);
    layoutPolicy->addItem(buttonsLayout, Qt::AlignCenter);
    layoutPolicy->addStretch();

    setLayout(layout);
}

void AccountSetupFinishedWidget::actionButtonClicked()
{
    QString serviceName = sender()->property("serviceName").toString();
    qDebug() << "Invoking service" << serviceName;
    AccountsUI::LastPageActions::executeService(serviceName);
    bool killCaller = true;
    QVariant data(killCaller);
    AccountsUI::ProviderPluginProcess::instance()->setExitData(data);
    AccountsUI::ProviderPluginProcess::instance()->quit();
}


