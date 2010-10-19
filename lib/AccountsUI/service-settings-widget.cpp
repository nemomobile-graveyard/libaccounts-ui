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

#include "service-settings-widget.h"
#include <AccountsUI/abstract-service-setup-context.h>
#include "settings-page.h"
#include "service-helper.h"
//MeegoTouch

#include <MContainer>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MLabel>
#include <MButton>
#include <QDebug>
#include <QDomElement>
namespace AccountsUI {
class ServiceSettingsWidgetPrivate
{
public:
    ServiceSettingsWidgetPrivate()
            : enableServiceButton(0)
            , context(0)
            , containerMainPolicy(0)
            , showAllSettings(0)
            {}
    ~ServiceSettingsWidgetPrivate() {}
    MButton *enableServiceButton;
    AbstractServiceSetupContext *context;
    MGridLayoutPolicy *containerMainPolicy;
    bool showAllSettings;
};

ServiceSettingsWidget::ServiceSettingsWidget(AbstractServiceSetupContext *context,
                                             QGraphicsItem *parent,
                                             bool showOnlySettings,
                                             bool showAllSettings,
                                             bool enabled)
    : MContainer(parent),
    d_ptr(new ServiceSettingsWidgetPrivate())
{
    Q_D(ServiceSettingsWidget);
    d->context = context;
    d->showAllSettings = showAllSettings;
    MWidget *containerCentralWidget = new MWidget(this);
    MLayout *containerMainLayout = new MLayout(containerCentralWidget);
    d->containerMainPolicy = new MGridLayoutPolicy(containerMainLayout);
    if (!showOnlySettings) {
        MLabel *serviceNameLabel = new MLabel(this);
        d->enableServiceButton = new MButton(this);
        d->enableServiceButton->setViewType(MButton::switchType);
        d->enableServiceButton->setCheckable(true);
        if (context) {
            ServiceHelper *serviceHepler =
                new ServiceHelper(const_cast<Accounts::Service*>(context->service()), this);
            serviceNameLabel->setText(serviceHepler->prettyName());

            /*
             * no signals during widget creation
             * */
            d->context->enable(enabled);
            d->enableServiceButton->setChecked(enabled);
            connect(d->enableServiceButton, SIGNAL(toggled(bool)), this, SLOT(enabled(bool)));

            d->containerMainPolicy->addItem(serviceNameLabel, 0, 0, Qt::AlignLeft);
            d->containerMainPolicy->addItem(d->enableServiceButton, 0, 1, Qt::AlignRight);
            if (!serviceHepler->description().isNull()) {
                MLabel *descLabel = new MLabel(serviceHepler->description());
                d->containerMainPolicy->addItem(descLabel, 1, 0, Qt::AlignLeft);
            }
        }
    }
    if (context) {
        MWidget *widget = context->widget(0, showAllSettings);
        if (widget) {
            d->containerMainPolicy->addItem(widget, 2, 0);
        }
    }
    setCentralWidget(containerCentralWidget);
}

ServiceSettingsWidget::~ServiceSettingsWidget()
{
    delete d_ptr;
}

void ServiceSettingsWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void ServiceSettingsWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(ServiceSettingsWidget);
    if (d->context) {
        if (d->context->widget(0))
            openSettingsPage();
    }
}

void ServiceSettingsWidget::enabled(bool enabled)
{
    Q_D(ServiceSettingsWidget);
    if (!d->context)
        return;

    d->context->enable(enabled);

    if (enabled &&
        d->context->service()) {
            emit serviceButtonEnabled(d->context->service()->serviceType());
            qDebug() << Q_FUNC_INFO << __LINE__ << d->context->service()->serviceType() << enabled;
    }
}

void ServiceSettingsWidget::setServiceButtonEnable(bool enable)
{
    Q_D(ServiceSettingsWidget);
    qDebug() << Q_FUNC_INFO << __LINE__;

    if (d->enableServiceButton->isChecked() != enable)
        d->enableServiceButton->setChecked(enable);
}

void ServiceSettingsWidget::openSettingsPage()
{
    Q_D(ServiceSettingsWidget);
    SettingsPage *settingsPage =
            new SettingsPage(d->context);
    settingsPage->appear();
}

}//end of namespace
