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

#include "add-account-page.h"

#include "service-selection-page.h"
#include "service-model.h"
#include "service-helper.h"
#include "sort-service-model.h"
#include "provider-plugin-process.h"

//libaccounts
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
};

AddAccountPage::AddAccountPage(AbstractAccountSetupContext *context,
                               QGraphicsItem *parent)
        : MApplicationPage(parent)
        , d_ptr(new AddAccountPagePrivate())
{
    Q_D(AddAccountPage);
    Q_ASSERT(context);
    setStyleName("AddAccountPage");
    setEscapeMode(MApplicationPageModel::EscapeAuto);
    d->context = context;
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
    // TODO : listen for rotation change and propagate it to the widget for correct layout policy
    connect(d->context, SIGNAL(validated()), SLOT(navigateToServiceSelectionPage())); // login Ok, go to next page
    //process indicator
    connect(d->context, SIGNAL(validating()), SLOT(startProgressIndicator()));
    connect(d->context, SIGNAL(error(AccountsUI::ErrorCode, const QString &)),
            this,  SLOT(stopProgressIndicator()));
}

void AddAccountPage::navigateToServiceSelectionPage()
{
    Q_D(AddAccountPage);
    disconnect(d->context, SIGNAL(validated()), this, SLOT(navigateToServiceSelectionPage()));

    ServiceModel *serviceModel = new ServiceModel(d->context->account(), this);

    SortServiceModel *sortModel = new SortServiceModel(this);
    sortModel->setSourceModel(serviceModel);
    sortModel->sort(ServiceModel::ServiceNameColumn);

    QAbstractProxyModel *proxy = 0;
    // selecting the service type
    if (!d->context->serviceType().isEmpty()) {
        FilterTypeServiceModel *filterServiceModel = new FilterTypeServiceModel(this);
        filterServiceModel->setSourceModel(sortModel);
        filterServiceModel->setFilterFixedString(d->context->serviceType());
        proxy = filterServiceModel;
    } else
        proxy = sortModel;

    for (int i = 0; i < proxy->rowCount(); i++) {
        QModelIndex index = proxy->index(i, 0);
        const QVariant vServiceHelper = index.data(ServiceModel::ServiceHelperColumn);
        ServiceHelper *serviceHelper = vServiceHelper.value<ServiceHelper *>();
        AbstractServiceSetupContext *context =
            serviceHelper->serviceSetupContext(d->context, this);
        d->serviceContextList.append(context);
    }

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

    ServiceSelectionPage *serviceSelectionPage =
        new ServiceSelectionPage(d->context, d->serviceContextList);
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
            stopProgressIndicator();
            break;
        case NotStored:
            qDebug() << Q_FUNC_INFO << __LINE__;
            stopProgressIndicator();
            break;
        case Validated:
            d->syncHandler->store(d->abstractContexts);
            break;
        case Stored:
            connect(d->context->account(), SIGNAL(synced()),
                    ProviderPluginProcess::instance(), SLOT(quit()));
            d->context->account()->sync();
            stopProgressIndicator();
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

void AddAccountPage::startProgressIndicator()
{
    qDebug() << Q_FUNC_INFO;
    setProgressIndicatorVisible(true);
}

void AddAccountPage::stopProgressIndicator()
{
    qDebug() << Q_FUNC_INFO;
    setProgressIndicatorVisible(false);
}

} //namespace
