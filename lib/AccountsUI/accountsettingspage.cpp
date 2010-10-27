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

#include "accountsettingspage.h"
#include "provider-plugin-process.h"
#include "service-settings-widget.h"
#include "AccountsUI/CredentialDialog"
#include "service-model.h"
#include "sort-service-model.h"

//accounts-qt lib
#include <Accounts/Account>

//Meegotouch
#include <MContainer>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MMessageBox>
#include <MLocale>
#include <MAction>
#include <MLabel>
#include <MGridLayoutPolicy>
#include <MButton>
#include <MContentItem>
#include <MApplicationIfProxy>
#include <MMessageBox>

//Qt
#include <QDebug>
#include <QTimer>
#include <QMultiMap>

//Accounts-Ui
#include "abstract-account-setup-context.h"

#define INFO_BANNER_TIMEOUT 3000

namespace AccountsUI {

class AccountSettingsPagePrivate
{
public:
    AccountSettingsPagePrivate()
            : context(0),
            service(0),
            account(0),
            showAllServices(0),
            usernameAndStatus(0),
            serviceSettingLayout(0),
            layoutServicePolicy(0),
            enableButton(0),
            syncHandler(0),
            changePasswordDialogStarted(false)
    {}

    ~AccountSettingsPagePrivate() {}

    AbstractAccountSetupContext *context;
    const Accounts::Service *service;
    Accounts::Account* account;
    QList<AbstractServiceSetupContext *> contexts;
    MAction *showAllServices;
    MContentItem *usernameAndStatus;
    QString serviceType;
    MLayout *serviceSettingLayout;
    MGridLayoutPolicy *layoutServicePolicy;
    MButton *enableButton;
    Accounts::ServiceList serviceList;
    QList<AbstractSetupContext*> abstractContexts;
    AccountSyncHandler *syncHandler;
    bool changePasswordDialogStarted;
    QMultiMap<QString, ServiceSettingsWidget*> settingsWidgets;
};

void AccountSettingsPage::setServicesToBeShown()
{
    Q_D(AccountSettingsPage);
    /* List the services available on the account and load all the respective
     * plugins. */
    int row = 0;
    //% "%1 Settings"
    setTitle(qtTrId("qtn_acc_ser_prof_set_title").arg(d->context->account()->providerName()));

    ServiceModel *serviceModel = new ServiceModel(d->context->account(), this);
    SortServiceModel *sortModel = new SortServiceModel(this);
    sortModel->setSourceModel(serviceModel);
    sortModel->setEnabledServices(d->context->account()->enabledServices());
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
        d->contexts.append(context);
    }

    /* iterate through the contexts we created for each service, and get the
     * UI widgets to embed */
    QMap<QString, bool> enabledServiceTypes;

    foreach (AbstractServiceSetupContext *context, d->contexts) {
        d->abstractContexts.append(context);
        d->service = context->service();
        const Accounts::Service *service = context->service();
        ServiceSettingsWidget *settingsWidget;

        d->account->selectService(service);

        bool enabled = false;
        if (d->account->enabled() &&
            !enabledServiceTypes.contains(service->serviceType())) {
            enabledServiceTypes.insert(service->serviceType(), true);
            enabled = true;
        }
        if (d->serviceList.count() > 1)
            settingsWidget = new ServiceSettingsWidget(context,
                                                   this,
                                                   ShowButtonAndSettings,
                                                   ShowMandatory,
                                                   enabled);
        else
            settingsWidget = new ServiceSettingsWidget(context,
                                                       this,
                                                       ShowOnlySettings,
                                                       ShowAll,
                                                       enabled);

        d->settingsWidgets.insertMulti(service->serviceType(), settingsWidget);

        settingsWidget->setHeaderVisible(false);
        d->layoutServicePolicy->addItem(settingsWidget, row++, 0);
    }

    /*
     * no need in extra processing of any signals during content creation
     * */
    foreach (ServiceSettingsWidget *settingsWidget, d->settingsWidgets)
        connect (settingsWidget, SIGNAL(serviceButtonEnabled(const QString&)),
                 this, SLOT(disableSameServiceTypes(const QString&)));
}

AccountSettingsPage::AccountSettingsPage(AbstractAccountSetupContext *context)
        : MApplicationPage(),
          d_ptr(new AccountSettingsPagePrivate())
{
    Q_D(AccountSettingsPage);

    Q_ASSERT (context != NULL);
    d->context = context;
    d->account = d->context->account();
    d->abstractContexts.append(d->context);
    d->serviceType = d->context->serviceType();

    d->syncHandler = new AccountSyncHandler(this);
    connect(d->syncHandler, SIGNAL(syncStateChanged(const SyncState&)),
            this, SLOT(onSyncStateChanged(const SyncState&)));
    setStyleName("AccountSettingsPage");
}

AccountSettingsPage::~AccountSettingsPage()
{
    delete d_ptr;
}

void AccountSettingsPage::createContent()
{
    Q_D(AccountSettingsPage);

    //we need a central widget to get the right layout size under the menubar
    MWidget* centralWidget = new MWidget();
    MLayout* layout = new MLayout(centralWidget);
    MLinearLayoutPolicy *layoutPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);

    if (d->context) {
        QGraphicsLayoutItem *accountSettingsWidget = d->context->widget();
        if(accountSettingsWidget != 0) {
            layoutPolicy->addItem(accountSettingsWidget);
        } else {
            MWidget *upperWidget = new MWidget(this);
            MLayout *upperLayout = new MLayout(upperWidget);
            MLinearLayoutPolicy *upperLayoutPolicy = new MLinearLayoutPolicy(upperLayout, Qt::Vertical);

            MLayout *horizontalLayout = new MLayout();
            MLinearLayoutPolicy *horizontalLayoutPolicy = new MLinearLayoutPolicy(horizontalLayout, Qt::Horizontal);
            d->usernameAndStatus = new MContentItem(MContentItem::TwoTextLabels);
            horizontalLayoutPolicy->addItem(d->usernameAndStatus);
            d->serviceList = d->account->services();
            d->enableButton = new MButton();
            d->enableButton->setViewType(MButton::switchType);
            d->enableButton->setCheckable(true);
            d->account->selectService(NULL);
            if ( d->account->enabled())
                d->enableButton->setChecked(true);
            else
                d->enableButton->setChecked(false);
            connect(d->enableButton, SIGNAL(toggled(bool)), this, SLOT(enable(bool)));
            horizontalLayoutPolicy->addItem(d->enableButton);
            if (d->context->account()) {
                d->usernameAndStatus->setTitle(d->context->account()->displayName());
                d->context->account()->selectService(NULL);
                if (!d->context->account()->enabled()) {
                    //% "Disabled"
                    d->usernameAndStatus->setSubtitle(qtTrId("qtn_acc_disabled"));
                } else {
                    d->usernameAndStatus->setSubtitle(QString::null);
                }
            }
            upperLayoutPolicy->addItem(horizontalLayout);

            //% "Change password"
            MButton *changePassword = new MButton(qtTrId("qtn_acc_change_password"));
            upperLayoutPolicy->addItem(changePassword);
            connect(changePassword, SIGNAL(clicked()), this, SLOT(openChangePasswordDialog()));

            layoutPolicy->addItem(upperWidget);
        }
    }

    MWidget *serviceWidget = new MWidget(this);
    d->serviceSettingLayout = new MLayout(serviceWidget);
    d->layoutServicePolicy = new MGridLayoutPolicy(d->serviceSettingLayout);
    layoutPolicy->addItem(serviceWidget);

    /* Sets the service widgets and add it into the layout policy*/
    setServicesToBeShown();

    MContentItem *synchItem = new MContentItem(MContentItem::SingleTextLabel);
    //% "Synchronization"
    synchItem->setTitle(qtTrId("qtn_acc_sync"));
    layoutPolicy->addItem(synchItem);
    connect(synchItem, SIGNAL(clicked()),
            this, SLOT(openSynchUi()));

    layoutPolicy->addStretch();
    setCentralWidget(centralWidget);

    //% "Delete"
    MAction* action = new MAction(qtTrId("qtn_comm_command_delete"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);

    connect(action, SIGNAL(triggered()),
            this, SLOT(removeAccount()));

    if (d->context && !d->context->serviceType().isEmpty()) {
        QStringList serviceType;
        Accounts::ServiceList services = d->context->account()->services();
        foreach (Accounts::Service *service, services) {
            serviceType.append(service->serviceType());
        }
        serviceType.removeDuplicates();
        if (serviceType.count() > 1) {
            //% "All Services"
            d->showAllServices = new MAction(qtTrId("qtn_acc_settings_all_services"), this);
            d->showAllServices->setLocation(MAction::ApplicationMenuLocation);
            addAction(d->showAllServices);
            connect(d->showAllServices, SIGNAL(triggered()),
                    this, SLOT(showAllServices()));
        }
    }

    //Saving the settings on back button press
    connect(this, SIGNAL(backButtonClicked()),
            this, SLOT(saveSettings()));
}

const AbstractAccountSetupContext *AccountSettingsPage::context()
{
    Q_D(AccountSettingsPage);
    return d->context;
}

void AccountSettingsPage::enable(bool state)
{
    Q_D(AccountSettingsPage);

    if (d->serviceList.count() == 1) {
        d->account->selectService(d->serviceList.at(0));
        d->account->setEnabled(state);
    }

    d->context->account()->selectService(NULL);
    if (state) {
        if(d->usernameAndStatus)
            d->usernameAndStatus->setSubtitle(QString::null);
    } else {
        if(d->usernameAndStatus)
            //% "Disabled"
            d->usernameAndStatus->setSubtitle(qtTrId("qtn_acc_disabled"));
    }

    d->account->setEnabled(state);
}

void AccountSettingsPage::removeAccount()
{
    Q_D(AccountSettingsPage);
    //% "Delete %1 from your device?"
    QString dialogTitle =
        qtTrId("qtn_acc_remove_account").arg(d->context->account()->displayName());
    MMessageBox removeMBox(dialogTitle, M::YesButton | M::NoButton);
    removeMBox.setStyleName("RemoveDialog");

    if (removeMBox.exec() == M::YesButton) {
        d->context->account()->remove();
        d->context->account()->sync();
        ProviderPluginProcess::instance()->quit();
    }
}

void AccountSettingsPage::saveSettings()
{
    Q_D(AccountSettingsPage);
    setProgressIndicatorVisible(true);
    qDebug() << Q_FUNC_INFO;
    d->syncHandler->store(d->abstractContexts);
}

void AccountSettingsPage::onSyncStateChanged(const SyncState &state)
{
    qDebug() << Q_FUNC_INFO;

    Q_D(AccountSettingsPage);
    switch (state) {
        case NotValidated:
            qDebug() << Q_FUNC_INFO << "NotValidated";
            setProgressIndicatorVisible(false);
            break;
        case Validated:
            qDebug() << Q_FUNC_INFO << "Validated";
            d->syncHandler->store(d->abstractContexts);
            break;
        case NotStored:
            qDebug() << Q_FUNC_INFO << "NotStored";
            connect(d->context->account(), SIGNAL(synced()),
                    ProviderPluginProcess::instance(), SLOT(quit()));
            d->context->account()->sync();
            break;
        case Stored:
            qDebug() << Q_FUNC_INFO << "Stored";
            connect(d->context->account(), SIGNAL(synced()),
                    ProviderPluginProcess::instance(), SLOT(quit()));
            d->context->account()->sync();
            break;
        default:
            return;
    }
}

void AccountSettingsPage::openChangePasswordDialog()
{
    Q_D(AccountSettingsPage);
    //ignore multiple clicks
    if (d->changePasswordDialogStarted)
    {
        qDebug() << Q_FUNC_INFO << "Change password dialog is started already";
        return;
    }

    d->changePasswordDialogStarted = true;

    CredentialDialog *credentialDialog = new CredentialDialog(d->account->credentialsId());
    credentialDialog->setParent(this);

    if (!credentialDialog) {
        qCritical() << "Cannot create change password dialog";
        return;
    }
    connect (credentialDialog, SIGNAL(safeToDeleteMe(CredentialDialog*)),
             this, SLOT(deleteCredentialsDialog()));
    //% "Change Password"
    credentialDialog->setTitle(qtTrId("qtn_acc_login_title_change"));
    credentialDialog->exec();
}

void AccountSettingsPage::deleteCredentialsDialog()
{
    Q_D(AccountSettingsPage);
    d->changePasswordDialogStarted = false;
    CredentialDialog *credentialDialog;

    if (sender() != NULL &&
        (credentialDialog = qobject_cast<CredentialDialog *>(sender())) != NULL)
        credentialDialog->deleteLater();
}

void AccountSettingsPage::openSynchUi()
{
    setProgressIndicatorVisible(true);

    //Start Sync-Ui
    MApplicationIfProxy mApplicationIfProxy("com.nokia.syncui", this);

    if (mApplicationIfProxy.connection().isConnected()) {
        mApplicationIfProxy.launch();
    } else {
        MInfoBanner* infoBanner = new MInfoBanner();

        //% "Unable to launch Synchronisation"
        infoBanner->setBodyText(qtTrId("qtn_acc_synchronisation_err_undefined"));
        infoBanner->appear(MSceneWindow::DestroyWhenDone);
        QTimer::singleShot(INFO_BANNER_TIMEOUT, infoBanner, SLOT(disappear()));
    }
    setProgressIndicatorVisible(false);
}

void AccountSettingsPage::showAllServices()
{
    Q_D(AccountSettingsPage);
    d->serviceType = QLatin1String("");
    setServicesToBeShown();
    d->showAllServices->setVisible(false);
}


/*
 * The same serviceTypes cannot be enabled in meantime
 * */
void AccountSettingsPage::disableSameServiceTypes(const QString &serviceType)
{
    Q_D(AccountSettingsPage);
    qDebug() << Q_FUNC_INFO << __LINE__;
    if (!sender())
    {
        qCritical() << "disableSameServiceTypes() must be called via signaling";
        return;
    }

    if (d->settingsWidgets.count(serviceType) == 1)
        return;

    foreach (ServiceSettingsWidget *widget, d->settingsWidgets.values(serviceType)) {
        if (widget == sender())
            continue;

        widget->setServiceButtonEnable(false);
    }
}
} // namespace
