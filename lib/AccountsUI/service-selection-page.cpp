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
#include "provider-plugin-process.h"
#include "basic-header-widget.h"
#include "last-page-actions.h"

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
#include <MBasicListItem>
#include <MWidgetAction>
#include <MImageWidget>
#include <MLabel>
#include <MLocale>
#include <MPannableViewport>
#include <MPositionIndicator>

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
        layoutServicePolicy(0)
        {}
    ~ServiceSelectionPagePrivate() {}

    MList *serviceList;
    MAction *saveAction;
    MAction *cancelAction;
    AbstractAccountSetupContext *context;
    QList<AbstractServiceSetupContext*> serviceContextList;
    QList<AbstractSetupContext*> abstractContexts;
    AccountSyncHandler *syncHandler;
    MLinearLayoutPolicy *layoutServicePolicy;
    QString serviceType;
    QMap<QString, bool> serviceStatusMap;
};

ServiceSelectionPage::ServiceSelectionPage(AbstractAccountSetupContext *context,
                                           QList<AbstractServiceSetupContext*>
                                           &serviceContextList,
                                           QGraphicsItem *parent)
        : MApplicationPage(parent),
        d_ptr(new ServiceSelectionPagePrivate())
{
    Q_D(ServiceSelectionPage);
    setStyleName("AccountsUiPage");
    d->context = context;
    d->serviceType = context->serviceType();
    d->context->account()->selectService(NULL);
    d->context->account()->setEnabled(true);
    d->serviceContextList = serviceContextList;
    d->abstractContexts.append(d->context);

    d->syncHandler = new AccountSyncHandler(this);
    connect(d->syncHandler, SIGNAL(syncStateChanged(const SyncState&)),
            this, SLOT(onSyncStateChanged(const SyncState&)));
    pannableViewport()->positionIndicator()->setStyleName("CommonPositionIndicatorInverted");
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
    MLayout *layout = new MLayout(centralWidget);
    MLinearLayoutPolicy *layoutPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    layoutPolicy->setSpacing(0);

    if (d->context) {
        MWidget *upperWidget = new MWidget(this);
        MLayout *upperLayout = new MLayout(upperWidget);
        MLinearLayoutPolicy *upperLayoutPolicy = new MLinearLayoutPolicy(upperLayout, Qt::Vertical);
        upperLayoutPolicy->setSpacing(0);

        MLayout *topLayout = new MLayout();
        MLinearLayoutPolicy *topLayoutPolicy = new MLinearLayoutPolicy(topLayout, Qt::Vertical);
        topLayoutPolicy->setSpacing(0);

        QString providerName(d->context->account()->providerName());
        // xml file that describes the ui elements for the provider
        Accounts::Provider *provider = AccountsManager::instance()->provider(providerName);
        if (provider) {
            // loading common catalog to show device name string
            MLocale locale;
            QString catalog = "common";
            if (!catalog.isEmpty() && !locale.isInstalledTrCatalog(catalog)) {
                locale.installTrCatalog(catalog);
                MLocale::setDefault(locale);
            }

            // provider info header
            QDomElement root = provider->domDocument().documentElement();
            QDomElement providerIcon = root.firstChildElement("icon");
            QString providerIconId = providerIcon.text();

            BasicHeaderWidget *providerInfoItem = new BasicHeaderWidget(BasicHeaderWidget::IconWithTitle, this);
            providerInfoItem->createLayout();
            providerInfoItem->setImage(providerIconId);
            providerInfoItem->setTitle(qtTrId(provider->displayName().toLatin1()));
            providerInfoItem->setObjectName("wgServiceSelectionPageBasicListItem");
            topLayoutPolicy->addItem(providerInfoItem, Qt::AlignLeft | Qt::AlignVCenter);

            // account connected message
            QString accountConnectedMessageId;
            QDomElement accountConnectedMessage = root.firstChildElement("account-connected-message");
            if (!accountConnectedMessage.isNull()) {
                // display a account connected message for provider
                accountConnectedMessageId =
                    qtTrId((accountConnectedMessage.text()).toLatin1()).arg(productNameTr());
            } else {
                // default message for service selection.
                accountConnectedMessageId = qtTrId("qtn_acc_enable_services_in_apps");
            }
            MLabel *accountConnectedMessageLabel=
                    new MLabel(accountConnectedMessageId);
            accountConnectedMessageLabel->setWordWrap(1);
            accountConnectedMessageLabel->setStyleName("CommonBodyTextInverted");
            topLayoutPolicy->addItem(accountConnectedMessageLabel, Qt::AlignLeft | Qt::AlignVCenter);
        }

        MSeparator *separatorTop = new MSeparator(this);
        separatorTop->setStyleName("CommonHeaderDividerInverted");
        separatorTop->setOrientation(Qt::Horizontal);

        upperLayoutPolicy->addItem(topLayout);
        upperLayoutPolicy->addItem(separatorTop);

        layoutPolicy->addItem(upperWidget);
    }

    MWidget *serviceWidget = new MWidget();
    MLayout *serviceSettingLayout = new MLayout(serviceWidget);
    d->layoutServicePolicy = new MLinearLayoutPolicy(serviceSettingLayout, Qt::Vertical);
    d->layoutServicePolicy->setSpacing(0);

    for (int i = 0; i < d->serviceContextList.count(); i++) {
        bool serviceStatus = true;
        const Accounts::Service *service = d->serviceContextList.at(i)->service();
        QDomElement root = service->domDocument().documentElement();
        QDomElement serviceDefaultStatus = root.firstChildElement("default-status");
        if (serviceDefaultStatus.text() == "false")
            serviceStatus = false;
        //ServiceSettingsWidget sets the display widget of the changing settings
        ServiceSettingsWidget *settingsWidget =
            new ServiceSettingsWidget(d->serviceContextList.at(i), serviceWidget,
                                      ServiceSettingsWidget::MandatorySettings |
                                      ServiceSettingsWidget::EnableButton,
                                      serviceStatus);

        emit serviceEnabled(service->name(), true);
        d->serviceStatusMap.insert(service->name(), true);
        d->abstractContexts.append(d->serviceContextList.at(i));
        d->layoutServicePolicy->addItem(settingsWidget, Qt::AlignLeft);
        connect (settingsWidget, SIGNAL(serviceEnabled(const QString&, bool)),
                 this, SIGNAL(serviceEnabled(const QString&, bool)));
        connect (settingsWidget, SIGNAL(serviceEnabled(const QString&, bool)),
                 this, SLOT(setEnabledService(const QString&, bool)));
    }

    layoutPolicy->addItem(serviceWidget, Qt::AlignLeft);

    //% "DONE"
    d->saveAction = new MAction(qtTrId("qtn_comm_command_done"), centralWidget);
    d->saveAction->setLocation(MAction::ToolBarLocation);
    addAction(d->saveAction);

    //% "CANCEL"
    d->cancelAction = new MAction(qtTrId("qtn_comm_cancel"), centralWidget);
    d->cancelAction->setLocation(MAction::ToolBarLocation);
    addAction(d->cancelAction);
    layoutPolicy->addStretch();

    connect(d->saveAction, SIGNAL(triggered()),
            this, SLOT(onAccountInstallButton()));
    connect(d->cancelAction, SIGNAL(triggered()),
            this, SLOT(cancel()));
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

    setProgressIndicatorVisible(true);
    for (int i = 0; i < d->serviceContextList.count(); i++) {
        const Accounts::Service *service = d->serviceContextList.at(i)->service();
        QMap<QString, bool>::iterator mapIterator =
                d->serviceStatusMap.find(service->name());
        if (mapIterator == d->serviceStatusMap.end())
            continue;
        d->serviceContextList.at(i)->enable(mapIterator.value());
        d->serviceStatusMap.remove(mapIterator.key());
    }
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
            if (ProviderPluginProcess::instance()->lastPageActions().
                serviceActions().isEmpty()) {
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
                    this, SLOT(cancel()));
            setProgressIndicatorVisible(false);
            return;
    }
}

void ServiceSelectionPage::setWidget(MWidget *widget)
{
    Q_D(ServiceSelectionPage);

    if (d->layoutServicePolicy && widget)
        d->layoutServicePolicy->addItem(widget);
}

void ServiceSelectionPage::setEnabledService(const QString &serviceName,
                                            bool enabled)
{
    Q_D(ServiceSelectionPage);
    d->serviceStatusMap[serviceName] = enabled;
}

void ServiceSelectionPage::cancel()
{
    ProviderPluginProcess::instance()->setReturnToAccountsList(true);
}

} //namespace
