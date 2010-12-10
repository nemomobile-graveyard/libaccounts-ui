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
#include "accountsmanagersingleton.h"
#include "account-setup-finished-page.h"

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
#include <MSeparator>
#include <MContentItem>
#include <MWidgetAction>
#include <MImageWidget>

//libAccounts
#include <Accounts/Provider>
#include <Accounts/Manager>


namespace AccountsUI {

M_REGISTER_WIDGET_NO_CREATE(ServiceSelectionPage)

//TODO: write the service plugins
class ServiceSelectionPagePrivate
{
public:
    ServiceSelectionPagePrivate()
        : serviceList(0),
        saveAction(0),
        cancelAction(0),
        context(0),
        syncHandler(0),
        layout(0),
        layoutPolicy(0),
        mainLayoutPolicy(0)
        {}
    ~ServiceSelectionPagePrivate() {}

    MList *serviceList;
    MAction *saveAction;
    MAction *cancelAction;
    AbstractAccountSetupContext *context;
    QList<AbstractServiceSetupContext*> serviceContextList;
    QList<AbstractSetupContext*> abstractContexts;
    AccountSyncHandler *syncHandler;
    MLayout *layout;
    MLinearLayoutPolicy *layoutPolicy;
    MLinearLayoutPolicy *mainLayoutPolicy;
    QString serviceType;
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
    d->serviceType = context->serviceType();
    d->context->account()->selectService(NULL);
    d->context->account()->setEnabled(true);
    d->serviceContextList = serviceContextList;
    d->abstractContexts.append(d->context);

    setEscapeMode(MApplicationPageModel::EscapeManualBack);

    d->syncHandler = new AccountSyncHandler(this);
    connect(d->syncHandler, SIGNAL(syncStateChanged(const SyncState&)),
            this, SLOT(onSyncStateChanged(const SyncState&)));
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

    setComponentsDisplayMode(EscapeButton, MApplicationPageModel::Hide);

    //we need a central widget to get the right layout size under the menubar
    MWidget *centralWidget = new MWidget();
    setCentralWidget(centralWidget);
    d->layout = new MLayout(centralWidget);
    d->layoutPolicy = new MLinearLayoutPolicy(d->layout, Qt::Vertical);
    d->layoutPolicy->setSpacing(0);

    if (d->context) {
        MWidget *upperWidget = new MWidget(this);
        MLayout *upperLayout = new MLayout(upperWidget);
        MLinearLayoutPolicy *upperLayoutPolicy = new MLinearLayoutPolicy(upperLayout, Qt::Vertical);
        upperLayoutPolicy->setSpacing(0);

        MLayout *horizontalLayout = new MLayout();
        MLinearLayoutPolicy *horizontalLayoutPolicy = new MLinearLayoutPolicy(horizontalLayout, Qt::Horizontal);
        horizontalLayoutPolicy->setSpacing(0);

        QString providerName(d->context->account()->providerName());
        // xml file that describes the ui elements for the provider
        Accounts::Provider *provider = AccountsManager::instance()->provider(providerName);
        if (provider) {
            QDomElement root = provider->domDocument().documentElement();
            QDomElement providerIcon = root.firstChildElement("icon");
            QString providerIconId = providerIcon.text();

            // Provider info widgets
            MContentItem *providerInfoItem =
                new MContentItem(MContentItem::IconAndTwoTextLabels, this);
            providerInfoItem->setObjectName("pluginProviderName");
            providerInfoItem->setTitle(providerName);
            providerInfoItem->setSubtitle(d->context->account()->displayName());
            providerInfoItem->setImageID(providerIconId);
            horizontalLayoutPolicy->addItem(providerInfoItem, Qt::AlignLeft | Qt::AlignVCenter);
        }

        MSeparator *separatorTop = new MSeparator(this);
        separatorTop->setOrientation(Qt::Horizontal);

        upperLayoutPolicy->addItem(horizontalLayout);
        upperLayoutPolicy->addItem(separatorTop);

        d->layoutPolicy->addItem(upperWidget);
    }

    MWidget *serviceWidget = new MWidget();
    MLayout *serviceSettingLayout = new MLayout(serviceWidget);
    MLinearLayoutPolicy *layoutServicePolicy = new MLinearLayoutPolicy(serviceSettingLayout, Qt::Vertical);
    layoutServicePolicy->setSpacing(0);

    for (int i = 0; i < d->serviceContextList.count(); i++) {
        //ServiceSettingsWidget sets the display widget of the changing settings
        ServiceSettingsWidget *settingsWidget =
            new ServiceSettingsWidget(d->serviceContextList.at(i), serviceWidget,
                                      ServiceSettingsWidget::MandatorySettings |
                                      ServiceSettingsWidget::EnableButton,
                                      true);

        d->abstractContexts.append(d->serviceContextList.at(i));
        layoutServicePolicy->addItem(settingsWidget);
    }

    d->layoutPolicy->addItem(serviceWidget);

    MSeparator *separatorBottom = new MSeparator(this);
    separatorBottom->setOrientation(Qt::Horizontal);
    layoutServicePolicy->addItem(separatorBottom);

    //% "DONE"
    d->saveAction = new MAction(qtTrId("qtn_comm_command_done"), centralWidget);
    d->saveAction->setLocation(MAction::ToolBarLocation);
    addAction(d->saveAction);

    //% "CANCEL"
    d->cancelAction = new MAction(qtTrId("qtn_comm_cancel"), centralWidget);
    d->cancelAction->setLocation(MAction::ToolBarLocation);
    addAction(d->cancelAction);
    d->layoutPolicy->addStretch();

    connect(d->saveAction, SIGNAL(triggered()),
            this, SLOT(onAccountInstallButton()));
    connect(d->cancelAction, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(d->serviceList, SIGNAL(itemClicked(QModelIndex)),
            this,SLOT(serviceSelected(QModelIndex)));

    connect(this, SIGNAL(backButtonClicked()),
            this, SLOT(close()));
}

void ServiceSelectionPage::onAccountInstallButton()
{
    Q_D(ServiceSelectionPage);

    disconnect(d->saveAction, SIGNAL(triggered()),
               this, SLOT(onAccountInstallButton()));
    disconnect(d->cancelAction, SIGNAL(triggered()),
               this, SLOT(close()));

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
            if (d->serviceType.isEmpty()) {
                connect(d->context->account(), SIGNAL(synced()),
                        ProviderPluginProcess::instance(), SLOT(quit()));
                d->context->account()->sync();
            } else {
                d->context->account()->sync();
                AccountSetupFinishedPage *page = new AccountSetupFinishedPage(d->context);
                page->appear();
            }
            break;
        default:
            connect(d->saveAction, SIGNAL(triggered()),
                    this, SLOT(onAccountInstallButton()));
            connect(d->cancelAction, SIGNAL(triggered()),
                    this, SLOT(close()));
            setProgressIndicatorVisible(false);
            return;
    }
}

void ServiceSelectionPage::setWidget(MWidget *widget)
{
     Q_D(ServiceSelectionPage);

     if(d->mainLayoutPolicy && widget)
        d->mainLayoutPolicy->addItem(widget);
}

} //namespace
