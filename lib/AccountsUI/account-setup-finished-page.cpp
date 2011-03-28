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
#include <MPannableViewport>

//Qt
#include <QDebug>

//Accounts-Ui
#include "account-setup-finished-page-priv.h"
#include "accountsmanagersingleton.h"
#include "last-page-actions.h"
#include "provider-plugin-process.h"
#include "account-setup-finished-widget.h"

namespace AccountsUI {

AccountSetupFinishedPage::AccountSetupFinishedPage(AbstractAccountSetupContext *context)
        : MApplicationPage(),
          d_ptr(new AccountSetupFinishedPagePrivate())
{
    Q_D(AccountSetupFinishedPage);
    d->q_ptr = this;
    d->account = context->account();
    d->serviceType = context->serviceType();
    setObjectName("wgAccountSetupFinishedPage");
    setStyleName("AccountsUiPage");
    pannableViewport()->setVerticalPanningPolicy(MPannableViewport::PanningAsNeeded);
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

    AccountSetupFinishedWidget *widget = new AccountSetupFinishedWidget(d->account->providerName(), this);
    layoutPolicy->addItem(widget);
    setCentralWidget(centralWidget);
}

void AccountSetupFinishedPage::goToApplication()
{
    /* this slot is deprecated */
}

}
