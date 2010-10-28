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

#include "service-selection-page.h"
#include "account-sync-handler.h"
#include "provider-plugin-process.h"
#include "service-settings-widget.h"

//Qt

#include <QStringListModel>
#include <QItemSelection>
#include <QDebug>

//Meegotouch
#include <MLayout>
#include <MList>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MWidgetCreator>
#include <MContainer>

namespace AccountsUI {

M_REGISTER_WIDGET_NO_CREATE(ServiceSelectionPage)

//TODO: write the service plugins
class ServiceSelectionPagePrivate
{
public:
    ServiceSelectionPagePrivate()
        : serviceList(0),
        accountInstallButton(0),
        context(0),
        syncHandler(0)
        {}
    ~ServiceSelectionPagePrivate() {}
    MList *serviceList;
    MButton *accountInstallButton;
    AbstractAccountSetupContext *context;
    QList<AbstractServiceSetupContext*> serviceContextList;
    QList<AbstractSetupContext*> abstractContexts;
    AccountSyncHandler *syncHandler;
};

ServiceSelectionPage::ServiceSelectionPage(AbstractAccountSetupContext *context,
                                           QList<AbstractServiceSetupContext*>
                                           &serviceContextList,
                                           QGraphicsItem *parent)
        : MApplicationPage(parent),
        d_ptr(new ServiceSelectionPagePrivate())
{
    Q_D(ServiceSelectionPage);
    setStyleName("ServicePage");
    d->context = context;
    d->context->account()->selectService(NULL);
    d->context->account()->setEnabled(true);
    d->serviceContextList = serviceContextList;
    d->abstractContexts.append(d->context);
    QString providerName(d->context->account()->providerName());
    //% "Add new account"
    setTitle(qtTrId("qtn_acc_add_new_account_title"));
    setEscapeMode(MApplicationPageModel::EscapeManualBack);

    d->accountInstallButton = new MButton(this);
    d->accountInstallButton->setStyleName("serviceaccountInstallButton");
    //% "Finalize Setup"
    d->accountInstallButton->setText(qtTrId("qtn_comm_command_done"));

    d->syncHandler = new AccountSyncHandler(this);
    connect(d->syncHandler, SIGNAL(syncStateChanged(const SyncState&)),
            this, SLOT(onSyncStateChanged(const SyncState&)));
    connect(d->accountInstallButton,SIGNAL(clicked()),
            this, SLOT(onAccountInstallButton()));
    connect(d->serviceList,SIGNAL(itemClicked(QModelIndex)),
            this,SLOT(serviceSelected(QModelIndex)));
    connect(this, SIGNAL(backButtonClicked()),
            this, SLOT(close()));
}

void ServiceSelectionPage::serviceSelected(QModelIndex index)
{
    Q_UNUSED(index);
}

ServiceSelectionPage::~ServiceSelectionPage()
{
    delete d_ptr;
}

void ServiceSelectionPage::createContent()
{
    Q_D(ServiceSelectionPage);

    MWidget *centralWidget = new MWidget(this);
    MLayout *mainLayout = new MLayout(centralWidget);
    MLinearLayoutPolicy *mainLayoutPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Vertical);

    for (int i = 0; i < d->serviceContextList.count(); i++) {
        //ServiceSettingsWidget sets the display widget of the changing settings
        ServiceSettingsWidget *settingsWidget =
                new ServiceSettingsWidget(d->serviceContextList.at(i), this,
                                          MandatorySettings | EnableButton,
                                          true);
        settingsWidget->setHeaderVisible(false);
        mainLayoutPolicy->addItem(settingsWidget);

        d->abstractContexts.append(d->serviceContextList.at(i));
    }

    mainLayoutPolicy->setItemSpacing(d->serviceContextList.count(),20);
    mainLayoutPolicy->addItem(d->accountInstallButton);
    mainLayoutPolicy->addStretch();

    setCentralWidget(centralWidget);
}

void ServiceSelectionPage::onAccountInstallButton()
{
    Q_D(ServiceSelectionPage);
    disconnect(d->accountInstallButton,SIGNAL(clicked()),
               this, SLOT(onAccountInstallButton()));
    setProgressIndicatorVisible(true);
    d->syncHandler->validate(d->abstractContexts);
}

void ServiceSelectionPage::onSyncStateChanged(const SyncState &state)
{
    Q_D(ServiceSelectionPage);

    switch (state) {
        case Validated:
            d->syncHandler->store(d->abstractContexts);
            break;
        case Stored:
            connect(d->context->account(), SIGNAL(synced()),
                    ProviderPluginProcess::instance(), SLOT(quit()));
            d->context->account()->sync();
            break;
        default:
            connect(d->accountInstallButton,SIGNAL(clicked()),
                    this, SLOT(onAccountInstallButton()));
            setProgressIndicatorVisible(false);
            return;
    }
}

} //namespace
