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
#include "settings-page.h"
#include "abstract-service-setup-context.h"
#include "accountsmanagersingleton.h"
#include "service-helper.h"
#include "basic-header-widget.h"

//Accounts
#include <Accounts/Account>
#include <Accounts/Provider>

//M
#include <MWidget>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MLabel>
#include <MButton>
#include <MDetailedListItem>
#include <MSeparator>
#include <MImageWidget>
#include <MPannableViewport>
#include <MPositionIndicator>

//Qt
#include <QDebug>

namespace AccountsUI {

class SettingsPagePrivate
{
public:
    SettingsPagePrivate()
            : context(0)
            , enableButton(0),
            buttonModel(0)
    {}
    ~SettingsPagePrivate(){}
    AbstractServiceSetupContext *context;
    MButton *enableButton;
    MButtonModel *buttonModel;
};

SettingsPage::SettingsPage(AbstractServiceSetupContext *context,
                           MButtonModel *model,
                           QGraphicsItem *parent)
    : MApplicationPage(parent)
    , d_ptr(new SettingsPagePrivate())
{
    Q_D(SettingsPage);
    d->context = context;
    d->buttonModel = model;
    connect(this, SIGNAL(backButtonClicked()), d->context, SLOT(store()));
    pannableViewport()->positionIndicator()->setStyleName("CommonPositionIndicatorInverted");
}

SettingsPage::~SettingsPage()
{
    delete d_ptr;
}

void SettingsPage::createContent()
{
    Q_D(SettingsPage);

    setStyleName("AccountsUiPage");

    //% "%1 settings"
    setTitle(qtTrId("qtn_acc_ser_prof_set_title").arg(d->context->service()->name()));

    MWidget *upperWidget = new MWidget(this);
    MLayout *upperLayout = new MLayout(upperWidget);
    MLinearLayoutPolicy *upperLayoutPolicy = new MLinearLayoutPolicy(upperLayout, Qt::Vertical);
    upperLayoutPolicy->setSpacing(0);

    MLayout *horizontalLayout = new MLayout();
    MLinearLayoutPolicy *horizontalLayoutPolicy = new MLinearLayoutPolicy(horizontalLayout, Qt::Horizontal);

    QString providerName(d->context->account()->providerName());
    QString providerIconId;
    // xml file that describes the ui elements for the provider
    Accounts::Provider *provider = AccountsManager::instance()->provider(providerName);
    if (provider) {
        QDomElement root = provider->domDocument().documentElement();
        QDomElement providerIcon = root.firstChildElement("icon");
        providerIconId = providerIcon.text();
    }

    ServiceHelper *serviceHepler =
        new ServiceHelper(const_cast<Accounts::Service*>(d->context->service()), this);
    BasicHeaderWidget *usernameAndStatus = 0;

    if (d->context->account()->id() == 0) {
        usernameAndStatus = new BasicHeaderWidget(BasicHeaderWidget::IconWithTitle, this);
        usernameAndStatus->createLayout();
    } else {
        usernameAndStatus = new BasicHeaderWidget(BasicHeaderWidget::IconWithTitleAndSubTitle, this);
        usernameAndStatus->createLayout();
        usernameAndStatus->setSubtitle(qtTrId(d->context->account()->displayName().toLatin1()));
    }

    usernameAndStatus->setImage(providerIconId);
    usernameAndStatus->setTitle(serviceHepler->prettyName());
    usernameAndStatus->setSubtitle(serviceHepler->description());

    MSeparator *separatorTop = new MSeparator(this);
    separatorTop->setStyleName("CommonHeaderDividerInverted");
    separatorTop->setOrientation(Qt::Horizontal);

    d->context->account()->selectService(d->context->service());

    horizontalLayoutPolicy->addItem(usernameAndStatus, Qt::AlignLeft | Qt::AlignVCenter);
    upperLayoutPolicy->addItem(horizontalLayout);
    upperLayoutPolicy->addItem(separatorTop);

    MWidget *widget = d->context->widget(this);

    MWidget *centralWidget = new MWidget(this);
    MLayout *layout = new MLayout(centralWidget);
    MLinearLayoutPolicy *layoutPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    layoutPolicy->addItem(upperWidget);

    layoutPolicy->addItem(widget);
    layoutPolicy->addStretch();

    setCentralWidget(centralWidget);
}

}//end of namespace

