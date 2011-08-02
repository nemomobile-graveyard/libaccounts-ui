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
#include "service-settings-widget.h"
#include "abstract-service-setup-context.h"
#include "settings-page.h"
#include "service-helper.h"
#include "basic-header-widget.h"

//M
#include <MBasicListItem>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MLabel>
#include <MButton>
#include <MLocale>
#include <MImageWidget>

//Qt
#include <QDebug>
#include <QDomElement>

namespace AccountsUI {

ServiceSettingsWidgetListItem::ServiceSettingsWidgetListItem(QGraphicsWidget *parent)
        : MBasicListItem(MBasicListItem::IconWithTitleAndSubtitle, parent)
{
    setStyleName("CommonLargePanel");
    setObjectName("wgServiceSettingsWidgetListItem");

    horizontalLayout = new MLayout(this);

    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *horizontalLayoutPolicy
            = new MLinearLayoutPolicy(horizontalLayout, Qt::Horizontal);
    horizontalLayoutPolicy->setSpacing(0);

    MLayout *titleSubtitleLayout = new MLayout(horizontalLayout);
    titleSubtitleLayout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *titleSubtitleLayoutPolicy = new MLinearLayoutPolicy(titleSubtitleLayout, Qt::Vertical);
    titleSubtitleLayoutPolicy->setSpacing(0);

    titleLabelWidget()->setStyleName("CommonTitleInverted");
    titleSubtitleLayoutPolicy->addItem(titleLabelWidget(), Qt::AlignLeft | Qt::AlignTop);
    subtitleLabelWidget()->setStyleName("CommonSubTitleInverted");
    titleSubtitleLayoutPolicy->addItem(subtitleLabelWidget(), Qt::AlignLeft | Qt::AlignTop);

    horizontalLayoutPolicy->addItem(titleSubtitleLayout, Qt::AlignLeft | Qt::AlignCenter);

    subtitleLabelWidget()->setWordWrap(true);
    subtitleLabelWidget()->setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    subtitleLabelWidget()->setTextElide(false);

    MImageWidget *image = imageWidget();
    image->setImage("icon-m-common-drilldown-arrow-inverse");
    image->setStyleName("CommonDrillDownIcon");
    image->setObjectName("wgServiceSettingsWidgetDrillDownIcon");
    image->setVisible(false);
    horizontalLayoutPolicy->addItem(image, Qt::AlignCenter);
}

ServiceSettingsWidgetListItem::~ServiceSettingsWidgetListItem()
{}


QGraphicsLayout *ServiceSettingsWidgetListItem::createLayout()
{
    return horizontalLayout;
}

void ServiceSettingsWidgetListItem::showDrillDownButton()
{
    setStyleName("CommonLargePanelInverted");
    imageWidget()->setVisible(true);
}

class ServiceSettingsWidgetPrivate
{
public:
    ServiceSettingsWidgetPrivate()
        : context(0)
         ,enableServiceButton(0)
    {}

    ~ServiceSettingsWidgetPrivate()
    {}

    AbstractServiceSetupContext *context;
    MButton *enableServiceButton;
};

ServiceSettingsWidget::ServiceSettingsWidget(AbstractServiceSetupContext *context,
                                             QGraphicsItem *parent,
                                             int settingsConf,
                                             bool enabled)
    : MWidget(parent),
    d_ptr(new ServiceSettingsWidgetPrivate())
{
    Q_D(ServiceSettingsWidget);

    d->context = context;
    QString catalog = d->context->service()->trCatalog();
    if (!catalog.isEmpty()) {
         MLocale locale;
         locale.installTrCatalog(catalog);
         MLocale::setDefault(locale);
    }

    MLayout *containerMainLayout = new MLayout(this);
    MLinearLayoutPolicy *mainPolicy = new MLinearLayoutPolicy(containerMainLayout, Qt::Vertical);
    mainPolicy->setSpacing(0);
    mainPolicy->setContentsMargins(0,0,0,0);

    MWidget *upperWidget = new MWidget(this);
    MLayout *upperLayout = new MLayout(upperWidget);
    MLinearLayoutPolicy *containerMainPolicy = new MLinearLayoutPolicy(upperLayout, Qt::Horizontal);
    containerMainPolicy->setSpacing(0);
    containerMainPolicy->setContentsMargins(0,0,0,0);
    ServiceSettingsWidgetListItem *serviceInfoList = 0;
    BasicHeaderWidget *serviceInfoLayout = 0;

    if (settingsConf & EnableButton) {
        if (context) {
            d->enableServiceButton = new MButton(this);
            d->enableServiceButton->setViewType(MButton::switchType);
            d->enableServiceButton->setStyleName("CommonLeftSwitchInverted");
            d->enableServiceButton->setObjectName("wgServiceSettingsWidgetServiceButton");
            d->enableServiceButton->setCheckable(true);

            ServiceHelper *serviceHelper =
                new ServiceHelper(const_cast<Accounts::Service*>(context->service()), this);
            QString subTitle = serviceHelper->description();

            if (settingsConf != (MandatorySettings | EnableButton)) {
                if (!serviceHelper->shortDescription().isEmpty())
                    subTitle = serviceHelper->shortDescription();
            }

            MWidget *widget = context->widget(0);
            MWidget *serviceInfo = 0;
            if (widget) {
                serviceInfoList = new ServiceSettingsWidgetListItem();
                serviceInfoList->setTitle(serviceHelper->prettyName());
                serviceInfoList->setSubtitle(subTitle);
                serviceInfo = serviceInfoList;
            } else {
                serviceInfoLayout = new BasicHeaderWidget(MBasicListItem::TitleWithSubtitle, this);
                serviceInfoLayout->setTitle(serviceHelper->prettyName());
                serviceInfoLayout->setSubtitle(subTitle);
                serviceInfo = serviceInfoLayout;
            }

            /*
             * no signals during widget creation
             * */
            d->enableServiceButton->setChecked(enabled);
            connect(d->enableServiceButton, SIGNAL(toggled(bool)), this, SLOT(enabled(bool)));

            containerMainPolicy->addItem(d->enableServiceButton, Qt::AlignRight | Qt::AlignCenter);
            containerMainPolicy->addItem(serviceInfo, Qt::AlignLeft | Qt::AlignTop);

            mainPolicy->addItem(upperWidget);
        }
    }

    if (context) {
        MWidget *widget = context->widget(0, (settingsConf & NonMandatorySettings));

        if (widget) {
            if (serviceInfoList) {
                serviceInfoList->showDrillDownButton();
                connect(serviceInfoList, SIGNAL(clicked()),
                        this, SLOT(openSettingsPage()));
            }

            if ((settingsConf & NonMandatorySettings) ||
                (settingsConf & MandatorySettings)) {

                /* Ugly Ugly HACK -
                 * Remove ASAP + Add fix for AbstractServiceSetupContext API,
                 * in order to properly solve this issue. */
                if (context->account()->providerName() != QLatin1String("google"))
                    mainPolicy->addItem(widget);
            }
        }
    }
}

ServiceSettingsWidget::~ServiceSettingsWidget()
{
    delete d_ptr;
}

void ServiceSettingsWidget::enabled(bool enabled)
{
    Q_D(ServiceSettingsWidget);
    if (!d->context)
        return;

    emit serviceEnabled(d->context->service()->name(), enabled);
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
    if (d->enableServiceButton == 0) return;

    if (d->enableServiceButton->isChecked() != enable)
        d->enableServiceButton->setChecked(enable);
}

void ServiceSettingsWidget::openSettingsPage()
{
    Q_D(ServiceSettingsWidget);
    if (d->enableServiceButton == 0) return;

    SettingsPage *settingsPage =
            new SettingsPage(d->context, d->enableServiceButton->model());
    settingsPage->appear();
    QObject::connect(settingsPage, SIGNAL(disappearing()),
                     this, SLOT(onSettingsPageClosed()));
}

void ServiceSettingsWidget::onSettingsPageClosed()
{
    Q_D(ServiceSettingsWidget);
    if (d->enableServiceButton == 0) return;

    // Restore button style
    d->enableServiceButton->setStyleName("CommonLeftSwitchInverted");
}

}//end of namespace
