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

//project
#include "add-account-page.h"
#include "service-selection-page.h"
#include "service-model.h"
#include "service-helper.h"
#include "sort-service-model.h"
#include "provider-plugin-process.h"
#include "account-sync-handler.h"
#include "genericaccountsetupform.h"
#include "account-setup-finished-page.h"

//Accounts
#include <Accounts/Account>
#include <Accounts/Manager>

//Meegotouch
#include <MLocale>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MImageWidget>
#include <MButton>
#include <MWidgetCreator>

//Qt
#include <QString>
#include <QSortFilterProxyModel>
#include <QRegExp>
#include <QDebug>

using namespace Accounts;

M_REGISTER_WIDGET_NO_CREATE(AccountsUI::AddAccountPage)

namespace AccountsUI {

class AddAccountPagePrivate
{
public:
    AddAccountPagePrivate()
            : context(0),
            syncHandler(0)
    {}
    ~AddAccountPagePrivate()
    {
        qDeleteAll(serviceContextList);
    }
    AbstractAccountSetupContext *context;
    QList<AbstractServiceSetupContext*> serviceContextList;
    AccountSyncHandler *syncHandler;
    QList<AbstractSetupContext*> abstractContexts;
    QString serviceType;
    Accounts::ServiceList hiddenServiceList;
};

AddAccountPage::AddAccountPage(AbstractAccountSetupContext *context,
                               QGraphicsItem *parent)
        : MApplicationPage(parent)
        , d_ptr(new AddAccountPagePrivate())
{
    Q_D(AddAccountPage);
    Q_ASSERT(context);
    setStyleName("AccountsUiPage");
    setEscapeMode(MApplicationPageModel::EscapeAuto);
    d->context = context;
    d->serviceType = context->serviceType();
}

AddAccountPage::~AddAccountPage()
{
    Q_D(AddAccountPage);
    delete d;
}

void AddAccountPage::createContent()
{
    Q_D(AddAccountPage);
    Q_ASSERT(centralWidget());

    //% "Add new account"
    setTitle(qtTrId("qtn_acc_add_new_account_title"));

    // layout
    MLayout *layout = new MLayout(centralWidget());
    MLinearLayoutPolicy *layoutPolicy =
            new MLinearLayoutPolicy( layout, Qt::Vertical );

    // plugin widget has the provider info and credentials widget
    QGraphicsLayoutItem *pluginWidget = d->context->widget();
    layoutPolicy->addItem(pluginWidget);

    // TODO : this part is just for testing purposes, to jump to service selection page, without going through authentication
    if (!qgetenv("ACCOUNTSUI_SKIP_VALIDATION").isEmpty()) {
        MButton *nextButton = new MButton("Skip Validation");
        connect(nextButton, SIGNAL(clicked()), this, SLOT(navigateToServiceSelectionPage()));
        layoutPolicy->addItem(nextButton);
    }

    // login Ok, go to next page
    connect(d->context, SIGNAL(validated()), SLOT(navigateToServiceSelectionPage()));
    connect(d->context, SIGNAL(validated()), SLOT(showMenuBar()));

    //process indicator
    connect(d->context, SIGNAL(validating()), SLOT(hideMenuBar()));
    connect(d->context, SIGNAL(error(AccountsUI::ErrorCode, const QString &)),
            this, SLOT(onError(AccountsUI::ErrorCode, const QString &)));

    //cancelling
    connect((GenericAccountSetupForm*)pluginWidget, SIGNAL(stopButtonPressed()),
            d->context, SLOT(stopAuthSession()));
    connect((GenericAccountSetupForm*)pluginWidget, SIGNAL(stopButtonPressed()),
            d->context, SLOT(showMenuBar()));
}

void AddAccountPage::setHiddenServices(const Accounts::ServiceList &hiddenServices)
{
    Q_D(AddAccountPage);
    d->hiddenServiceList = hiddenServices;
}

void AddAccountPage::navigateToServiceSelectionPage()
{
    Q_D(AddAccountPage);
    disconnect(d->context, SIGNAL(validated()), this, SLOT(navigateToServiceSelectionPage()));

    ServiceModel *serviceModel = new ServiceModel(d->context->account(), this);

    SortServiceModel *sortModel = new SortServiceModel(this);
    sortModel->setSourceModel(serviceModel);
    sortModel->setHiddenServices(d->hiddenServiceList);
    sortModel->sort(ServiceModel::ServiceNameColumn);

    d->serviceContextList = ServiceModel::createServiceContexts(sortModel, d->context, this);

    if (d->serviceContextList.count() == 0 ||
        (d->serviceContextList.count() == 1 &&
        !d->serviceContextList.at(0)->hasMandatorySettings())) {

        d->syncHandler = new AccountSyncHandler(this);
        connect(d->syncHandler, SIGNAL(syncStateChanged(const SyncState&)),
                this, SLOT(onSyncStateChanged(const SyncState&)));
        d->context->account()->selectService(NULL);
        d->context->account()->setEnabled(true);

        d->abstractContexts.append(d->context);
        if (d->serviceContextList.count() == 1) {
            d->context->account()->selectService(d->serviceContextList.at(0)->service());
            d->context->account()->setEnabled(true);
            d->abstractContexts.append(d->serviceContextList.at(0));
        }

        setProgressIndicatorVisible(true);

        qDebug() << Q_FUNC_INFO;
        d->syncHandler->validate(d->abstractContexts);
        return;
    }

    openServiceSelectionPage(d->context, d->serviceContextList);
}

void AddAccountPage::openServiceSelectionPage(AccountsUI::AbstractAccountSetupContext *context,
                                              QList<AccountsUI::AbstractServiceSetupContext *> &serviceContextList)
{
    ServiceSelectionPage *serviceSelectionPage =
        new ServiceSelectionPage(context, serviceContextList);
    connect(serviceSelectionPage,SIGNAL(backButtonClicked()),
            this,SLOT(appear()));
    connect(serviceSelectionPage,SIGNAL(backButtonClicked()),
            this, SLOT(clearServiceContextList()));
    connect(serviceSelectionPage,SIGNAL(backButtonClicked()),
            serviceSelectionPage,SLOT(deleteLater()));

    serviceSelectionPage->appear();
}

void AddAccountPage::onSyncStateChanged(const SyncState &state)
{

    qDebug() << Q_FUNC_INFO;
    Q_D(AddAccountPage);

    switch (state) {
        case NotValidated:
            qDebug() << Q_FUNC_INFO << __LINE__;
            showMenuBar();
            break;
        case NotStored:
            qDebug() << Q_FUNC_INFO << __LINE__;
            showMenuBar();
            break;
        case Validated:
            d->syncHandler->store(d->abstractContexts);
            break;
        case Stored:
            if (d->serviceType.isEmpty()) {
                connect(d->context->account(), SIGNAL(synced()),
                        ProviderPluginProcess::instance(), SLOT(quit()));
                d->context->account()->sync();
                showMenuBar();
            } else {
                d->context->account()->sync();
                AccountSetupFinishedPage *page = new AccountSetupFinishedPage(d->context);
                page->appear();
            }
            break;
        default:
            return;
    }
}

void AddAccountPage::clearServiceContextList()
{
    Q_D(AddAccountPage);
    d->serviceContextList.clear();
}

void AddAccountPage::hideMenuBar()
{
    setComponentsDisplayMode(NavigationBar, MApplicationPageModel::Hide);
}

void AddAccountPage::showMenuBar()
{
    setComponentsDisplayMode(NavigationBar, MApplicationPageModel::Show);
}

void AddAccountPage::onError(AccountsUI::ErrorCode, const QString &)
{
    showMenuBar();
}

} //namespace
