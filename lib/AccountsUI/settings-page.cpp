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

#include "settings-page.h"

#include <MWidget>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MLabel>
#include <MButton>
#include <AccountsUI/abstract-service-setup-context.h>
#include <QDebug>

namespace AccountsUI {
class SettingsPagePrivate
{
public:
    SettingsPagePrivate()
            : context(0)
            , enableAccountButton(0)
    {}
    ~SettingsPagePrivate(){}
    AbstractServiceSetupContext *context;
    MButton *enableAccountButton;
};

SettingsPage::SettingsPage(AbstractServiceSetupContext *context,
                           QGraphicsItem *parent)
    : MApplicationPage(parent)
    , d_ptr(new SettingsPagePrivate())
{
    Q_D(SettingsPage);
    d->context = context;
    connect(this, SIGNAL(backButtonClicked()), d->context, SLOT(store()));
}

SettingsPage::~SettingsPage()
{
    delete d_ptr;
}

void SettingsPage::createContent()
{
    Q_D(SettingsPage);
    //% "%1 settings"
    setTitle(qtTrId("qtn_acc_ser_prof_set_title").arg(d->context->service()->name()));
    MWidget *centralWidget = new MWidget(this);
    MLayout *layout = new MLayout(centralWidget);
    MLinearLayoutPolicy *layoutPolicy =
            new MLinearLayoutPolicy( layout, Qt::Vertical );
    MLayout *horizontalLayout = new MLayout(layout);
    MLinearLayoutPolicy *horizontalLayoutPolicy =
            new MLinearLayoutPolicy(horizontalLayout, Qt::Horizontal);

    //% "Enable %1"
    MLabel *enableLabel = new MLabel(qtTrId("qtn_acc_enable").
                                     arg(d->context->account()->displayName()));
    d->enableAccountButton = new MButton(this);
    d->enableAccountButton->setViewType(MButton::switchType);
    d->enableAccountButton->setCheckable(true);
    d->enableAccountButton->setChecked(true);
    connect(d->enableAccountButton, SIGNAL(toggled(bool)),
            this, SLOT(changeAccountStatus(bool)));

    horizontalLayoutPolicy->addItem(enableLabel);
    horizontalLayoutPolicy->addItem(d->enableAccountButton);
    horizontalLayoutPolicy->setSpacing(0);
    layoutPolicy->addItem(horizontalLayout);
    MWidget *widget = d->context->widget(this);
    layoutPolicy->addItem(widget);
    layoutPolicy->addStretch();
    setCentralWidget(centralWidget);
}

void SettingsPage::changeAccountStatus(bool status)
{
    Q_D(SettingsPage);
    d->context->account()->selectService(NULL);
    d->context->account()->setEnabled(status);
}

}//end of namespace

