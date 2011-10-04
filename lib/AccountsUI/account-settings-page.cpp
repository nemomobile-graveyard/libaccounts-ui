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
#include "account-settings-page-priv.h"
#include "provider-plugin-process.h"
#include "service-settings-widget.h"
#include "credentialdialog.h"
#include "service-model.h"
#include "sort-service-model.h"
#include "avatar-selector.h"

//Accounts
#include <Accounts/Account>
#include <Accounts/Provider>

//Telepathy
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/AccountSet>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/PendingContactInfo>
#include <TelepathyQt4/PendingContacts>
#include <TelepathyQt4/PendingHandles>
#include <TelepathyQt4/PendingVariant>
#include <TelepathyQt4/ConnectionLowlevel>
#include <TelepathyQt4/AvatarSpec>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Constants>

//Meegotouch
#include <MLayout>
#include <MMessageBox>
#include <MAction>
#include <MButton>
#include <MImageWidget>
#include <MSeparator>
#include <MLocale>
#include <MPannableViewport>
#include <MPositionIndicator>
#include <MBasicListItem>
#include <MImageWidget>

#include "basic-header-widget.h"
#include "avatar-selector.h"

//Qt
#include <QDebug>
#include <QBuffer>

#include <sysinfo.h>

#define INFO_BANNER_TIMEOUT 3000

using namespace AccountsUI;

AvatarListItem::AvatarListItem(QGraphicsWidget *parent)
        : MBasicListItem(MBasicListItem::IconWithTitle, parent)
{
    setStyleName("CommonLargePanel");
    setObjectName("wgServiceSettingsWidgetListItem");

    horizontalLayout = new MLayout(this);

    horizontalLayout->setContentsMargins(7, 0, 0, 0);
    MLinearLayoutPolicy *horizontalLayoutPolicy
            = new MLinearLayoutPolicy(horizontalLayout, Qt::Horizontal);
    horizontalLayoutPolicy->setSpacing(0);

    MLayout *titleSubtitleLayout = new MLayout(horizontalLayout);
    titleSubtitleLayout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *titleSubtitleLayoutPolicy = new MLinearLayoutPolicy(titleSubtitleLayout, Qt::Vertical);
    titleSubtitleLayoutPolicy->setSpacing(0);

    titleLabelWidget()->setStyleName("CommonTitleInverted");
    titleSubtitleLayoutPolicy->addItem(titleLabelWidget(), Qt::AlignLeft | Qt::AlignTop);

    avatarImage = new MImageWidget();
    avatarImage->setStyleName("CommonMainIcon");
    horizontalLayoutPolicy->addItem(avatarImage, Qt::AlignLeft);
    horizontalLayoutPolicy->addItem(titleSubtitleLayout, Qt::AlignLeft | Qt::AlignCenter);

    MImageWidget *image = imageWidget();
    image->setImage("icon-m-common-drilldown-arrow-inverse");
    image->setStyleName("CommonDrillDownIcon");
    image->setObjectName("wgServiceSettingsWidgetDrillDownIcon");
    image->setVisible(true);
    horizontalLayoutPolicy->addItem(image, Qt::AlignCenter);
}

AvatarListItem::~AvatarListItem()
{}

QGraphicsLayout *AvatarListItem::createLayout()
{
    return horizontalLayout;
}

void AvatarListItem::setImage(const QImage &image)
{
    avatarImage->setImage(image);
}

void AvatarListItem::setImage(const QString &id)
{
    avatarImage->setImage(id);
}

AccountSettingsPagePrivate::AccountSettingsPagePrivate(
    AbstractAccountSetupContext *context):
    context(context),
    account(0),
    enableButton(0),
    syncHandler(0),
    changePasswordDialogStarted(false),
    panel(0),
    layout(0),
    panelPolicy(0),
    saving(false),
    preferredWidth(avatar_width_medium),
    preferredHeight(avatar_height_medium)
{
    account = context->account();
    serviceList = account->services();
    abstractContexts.append(context);
    panel = new MWidgetController();
    syncHandler = new AccountSyncHandler(this);
    connect(syncHandler, SIGNAL(syncStateChanged(const SyncState&)),
            this, SLOT(onSyncStateChanged(const SyncState&)));
}

bool AccountSettingsPagePrivate::hasSingleService() const
{
    return serviceList.count() <= 1;
}

void AccountSettingsPagePrivate::saveSettings()
{
    Q_Q(AccountSettingsPage);

    if (saving) return;
    saving = true;

    q->setProgressIndicatorVisible(true);
    qDebug() << Q_FUNC_INFO;
    if (enableButton) {
        bool state = enableButton->isChecked();
        if (serviceList.count() == 1) {
            account->selectService(serviceList.at(0));
            if (account->enabled() != state)
                account->setEnabled(state);
        } else if (serviceList.count() > 1) {
            foreach (AbstractServiceSetupContext *serviceContext, contexts) {
                const Accounts::Service *service = serviceContext->service();
                QMap<QString, bool>::iterator i =
                        serviceStatusMap.find(service->name());
                if (i == serviceStatusMap.end())
                    continue;
                account->selectService(service);
                if (account->enabled() != i.value())
                        serviceContext->enable(i.value());
                serviceStatusMap.remove(i.key());
            }
        }

        context->account()->selectService(NULL);
        if (account->enabled() != state)
            account->setEnabled(state);
    }
    if (accountPtr) {
        if (accountPtr->isOnline()) {
            Tp::Avatar newAvatar;
            newAvatar.avatarData = avatarImageData;
            newAvatar.MIMEType = avatarImageType;
            connect((Tp::Account *)accountPtr.data(), SIGNAL(avatarChanged (const Tp::Avatar &)),
                    this, SLOT(onAvatarChange(const Tp::Avatar &)));
            op = accountPtr->setAvatar(newAvatar);
            connect(op, SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(finishedCalled(Tp::PendingOperation *)));
        } else
            qDebug() << "Account Not Online";
    }

    //we should call only validate. Storing will be handled
    //in onSyncStateChangted func.
    syncHandler->validate(abstractContexts);
}

void AccountSettingsPagePrivate::finishedCalled(Tp::PendingOperation *op)
{
    qDebug() << Q_FUNC_INFO;
    if (op->isError())
        qDebug() << op->errorMessage();
}

void AccountSettingsPagePrivate::onSyncStateChanged(const SyncState &state)
{
    qDebug() << Q_FUNC_INFO;

    Q_Q(AccountSettingsPage);
    switch (state) {
        case NotValidated:
            qDebug() << Q_FUNC_INFO << "NotValidated";
            q->setProgressIndicatorVisible(false);
            //Saving the settings on back button press
            saving = false;
            break;
        case Validated:
            qDebug() << Q_FUNC_INFO << "Validated";
            syncHandler->store(abstractContexts);
            break;
        case NotStored:
            qDebug() << Q_FUNC_INFO << "NotStored";
            connect(context->account(), SIGNAL(synced()),
                    ProviderPluginProcess::instance(), SLOT(quit()));
            context->account()->sync();
            break;
        case Stored:
            qDebug() << Q_FUNC_INFO << "Stored";
            connect(context->account(), SIGNAL(synced()),
                    ProviderPluginProcess::instance(), SLOT(quit()));
            context->account()->sync();
            break;
        default:
            return;
    }
}

void AccountSettingsPagePrivate::openChangePasswordDialog()
{
    //ignore multiple clicks
    if (changePasswordDialogStarted)
    {
        qDebug() << Q_FUNC_INFO << "Change password dialog is started already";
        return;
    }

    changePasswordDialogStarted = true;

    CredentialDialog *credentialDialog = new CredentialDialog(account->credentialsId());
    if (!credentialDialog) {
        qCritical() << "Cannot create change password dialog";
        return;
    }
    credentialDialog->setParent(this);
    connect (credentialDialog, SIGNAL(safeToDeleteMe(CredentialDialog*)),
             this, SLOT(deleteCredentialsDialog()));
    //% "Change Password"
    credentialDialog->setTitle(qtTrId("qtn_acc_login_title_change"));
    credentialDialog->exec();
}

void AccountSettingsPagePrivate::deleteCredentialsDialog()
{
    changePasswordDialogStarted = false;
    CredentialDialog *credentialDialog;

    if (sender() != NULL &&
        (credentialDialog = qobject_cast<CredentialDialog *>(sender())) != NULL)
        credentialDialog->deleteLater();
}


/*
 * The same serviceTypes cannot be enabled in meantime
 * */
void AccountSettingsPagePrivate::disableSameServiceTypes(const QString &serviceType)
{
    qDebug() << Q_FUNC_INFO << __LINE__;
    if (!sender())
    {
        qCritical() << "disableSameServiceTypes() must be called via signaling";
        return;
    }

    if (settingsWidgets.count(serviceType) == 1)
        return;

    foreach (ServiceSettingsWidget *widget, settingsWidgets.values(serviceType)) {
        if (widget == sender())
            continue;

        widget->setServiceButtonEnable(false);
    }
}

void AccountSettingsPagePrivate::setEnabledService(const QString &serviceName,
                                                   bool enabled)
{
    serviceStatusMap[serviceName] = enabled;
}

void AccountSettingsPagePrivate::scaleImage(const QImage &image, QImage &scaledImage)
{
    qDebug() << "Org width/ height" << image.width() << image.height();
    if (image.width() > preferredWidth
        || image.height() > preferredHeight) {
        qDebug() << "The loaded image needs scaling DOWN";
        scaledImage = image.scaled(preferredWidth, preferredHeight,
                                   Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        qDebug() << "The loaded image needs scaling UP";
        scaledImage = image.scaled(preferredWidth, preferredHeight,
                                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

}

void AccountSettingsPagePrivate::setAvatarImage(const QImage &scaledImg)
{
    QSize preferredSize(preferredWidth, preferredHeight);
    QPixmap *maskPixmap = MTheme::pixmapCopy(QLatin1String("meegotouch-contactsui-avatar-mask"),
                                             preferredSize);

    QPixmap *framePixmap = MTheme::pixmapCopy(QLatin1String("meegotouch-contactsui-avatar-frame"),
                                              preferredSize);

    if (maskPixmap != NULL && framePixmap != NULL) {

        qDebug() << "Non null Mask Pixmap && frame pixmap";
        QImage maskImage = maskPixmap->toImage();
        QImage frameImage = framePixmap->toImage();

        QImage compositeImage(preferredSize, QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&compositeImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(0, 0, scaledImg);

        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.drawImage(0, 0, maskImage);

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(0, 0, frameImage);
        painter.end();

        avatarItem->setImage(compositeImage);

        delete maskPixmap;
        delete framePixmap;

    } else {
        avatarItem->setImage(scaledImg);
    }
}

AccountSettingsPage::AccountSettingsPage(AbstractAccountSetupContext *context)
        : MApplicationPage(),
          d_ptr(new AccountSettingsPagePrivate(context))
{
    Q_D(AccountSettingsPage);

    Q_ASSERT (context != NULL);
    d->q_ptr = this;

    setStyleName("AccountsUiPage");
    pannableViewport()->positionIndicator()->setStyleName("CommonPositionIndicatorInverted");
}

AccountSettingsPage::~AccountSettingsPage()
{
    delete d_ptr;
}

void AccountSettingsPage::setServicesToBeShown()
{
    qDebug() << Q_FUNC_INFO << "Deprecated. This function does nothing.";
}

QGraphicsLayoutItem *AccountSettingsPage::createServiceSettingsLayout()
{
    Q_D(AccountSettingsPage);

    MWidget *serviceWidget = new MWidget(this);
    MLayout *serviceSettingLayout = new MLayout(serviceWidget);
    serviceSettingLayout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *layoutServicePolicy =
        new MLinearLayoutPolicy(serviceSettingLayout, Qt::Vertical);
    layoutServicePolicy->setSpacing(0);

    /* List the services available on the account and load all the respective plugins. */

    //% "%1 Settings"
    setTitle(qtTrId("qtn_acc_ser_prof_set_title").arg(d->context->account()->providerName()));

    ServiceModel *serviceModel = new ServiceModel(d->context->account(), this);
    SortServiceModel *sortModel = new SortServiceModel(this);
    sortModel->setSourceModel(serviceModel);
    sortModel->setEnabledServices(d->context->account()->enabledServices());
    sortModel->setHiddenServices(d->hiddenServiceList);
    sortModel->sort(ServiceModel::ServiceNameColumn);

    d->contexts = ServiceModel::createServiceContexts(sortModel, d->context, this);

    /* iterate through the contexts we created for each service, and get the
     * UI widgets to embed */
    QMap<QString, bool> enabledServiceTypes;

    foreach (AbstractServiceSetupContext *context, d->contexts) {
        d->abstractContexts.append(context);
        const Accounts::Service *service = context->service();
        ServiceSettingsWidget *settingsWidget;

        d->account->selectService(service);
        d->serviceStatusMap.insert(service->name(), d->account->enabled());
        emit serviceEnabled(service->name(), d->account->enabled());
        bool enabled = false;
        if (d->account->enabled() &&
            !enabledServiceTypes.contains(service->serviceType())) {
            enabledServiceTypes.insert(service->serviceType(), true);
            enabled = true;
        }

        if (!d->hasSingleService())
            settingsWidget = new ServiceSettingsWidget(context,
                                                   d->panel,
                                                   ServiceSettingsWidget::EnableButton,
                                                   enabled);
        else
            settingsWidget = new ServiceSettingsWidget(context,
                                                       d->panel,
                                                       ServiceSettingsWidget::MandatorySettings |
                                                       ServiceSettingsWidget::NonMandatorySettings,
                                                       enabled);

        d->settingsWidgets.insertMulti(service->serviceType(), settingsWidget);

        d->panelPolicy->addItem(settingsWidget);
    }

    d->panelPolicy->setSpacing(0);
    layoutServicePolicy->addItem(d->panel);
    /*
     * no need in extra processing of any signals during content creation
     * */

    foreach (ServiceSettingsWidget *settingsWidget, d->settingsWidgets) {
        connect(settingsWidget, SIGNAL(serviceButtonEnabled(const QString&)),
                d, SLOT(disableSameServiceTypes(const QString&)));
        connect(settingsWidget, SIGNAL(serviceEnabled(const QString&, bool)),
                d, SIGNAL(serviceEnabled(const QString&, bool)));
        connect(settingsWidget, SIGNAL(serviceEnabled(const QString&, bool)),
                d, SLOT(setEnabledService(const QString&, bool)));
    }

    return serviceWidget;
}

QGraphicsLayoutItem *AccountSettingsPage::createAccountSettingsLayout()
{
    Q_D(AccountSettingsPage);

    // First, see if the plugin has own implementation of this widget
    QGraphicsLayoutItem *accountSettingsWidget = d->context->widget();
    if (accountSettingsWidget != 0)
        return accountSettingsWidget;

    // Generic implementation
    MWidget *upperWidget = new MWidget(this);
    MLayout *upperLayout = new MLayout(upperWidget);
    upperLayout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *upperLayoutPolicy =
        new MLinearLayoutPolicy(upperLayout, Qt::Vertical);
    upperLayoutPolicy->setSpacing(0);

    MLayout *horizontalLayout = new MLayout();
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *horizontalLayoutPolicy =
        new MLinearLayoutPolicy(horizontalLayout, Qt::Horizontal);
    horizontalLayoutPolicy->setSpacing(0);

    QString providerName(d->account->providerName());
    QString providerIconId;
    Accounts::Provider *provider =
        AccountsManager::instance()->provider(providerName);
    if (provider) {
        providerIconId = provider->iconName();
        QString catalog = provider->trCatalog();
        MLocale locale;
        if (!catalog.isEmpty() && !locale.isInstalledTrCatalog(catalog)) {
            locale.installTrCatalog(catalog);
            MLocale::setDefault(locale);
        }
    }

    BasicHeaderWidget *usernameAndStatus = new BasicHeaderWidget(BasicHeaderWidget::IconWithTitleAndSubTitle, this);
    usernameAndStatus->createLayout();
    usernameAndStatus->setImage(providerIconId);
    usernameAndStatus->setTitle(qtTrId(provider->displayName().toLatin1()));
    usernameAndStatus->setSubtitle(d->account->displayName());

    MSeparator *separatorTop = new MSeparator(this);
    separatorTop->setOrientation(Qt::Horizontal);
    separatorTop->setStyleName("CommonHeaderDividerInverted");

    d->enableButton = new MButton(this);
    d->enableButton->setViewType(MButton::switchType);
    d->enableButton->setStyleName("CommonRightSwitchInverted");
    d->enableButton->setCheckable(true);

    d->account->selectService(NULL);
    if (d->account->enabled()) {
        d->panel->setEnabled(true);
        d->enableButton->setChecked(true);
    } else {
        d->panel->setEnabled(false);
        d->enableButton->setChecked(false);
    }

    connect(d->enableButton, SIGNAL(toggled(bool)), this, SLOT(enable(bool)));

    horizontalLayoutPolicy->addItem(usernameAndStatus,
                                    Qt::AlignLeft | Qt::AlignVCenter);
    horizontalLayoutPolicy->addItem(d->enableButton,
                                    Qt::AlignRight | Qt::AlignVCenter);

    MWidgetController *spacer = new MWidgetController(this);
    spacer->setStyleName("CommonSpacer");
    upperLayoutPolicy->addItem(spacer);

    upperLayoutPolicy->addItem(horizontalLayout);

    spacer = new MWidgetController(this);
    spacer->setStyleName("CommonSmallSpacer");
    upperLayoutPolicy->addItem(spacer);

    upperLayoutPolicy->addItem(separatorTop);

    spacer = new MWidgetController(this);
    spacer->setStyleName("CommonSmallSpacer");
    upperLayoutPolicy->addItem(spacer);

    struct system_config *sc = 0;
    QByteArray name;

    if (sysinfo_init(&sc) == 0) {
        uint8_t *data = 0;
        unsigned long size = 0;

        if (sysinfo_get_value(sc, "/device/sw-release-ver",
                              &data, &size) == 0) {
            name = QByteArray((const char *)(data), size);
            free(data);
        }
        sysinfo_finish(sc);
    }

    QDomElement root = provider->domDocument().documentElement();
    d->avatar = root.firstChildElement("display-avatar");

    if (name.endsWith("003"))
        d->avatar.clear();

    if (d->avatar.text() == "true") {
        d->avatarItem = new AvatarListItem();
        connect(d->avatarItem, SIGNAL(clicked()), this, SLOT(changeAvatar()));
        //% Avatar
        d->avatarItem->setTitle(qtTrId("qtn_acc_avatar"));
        d->panelPolicy->addItem(d->avatarItem);
        d->avatarSelector = new AvatarSelector();

    }

    return upperWidget;
}


void AccountSettingsPage::getAvatar()
{
    Q_D(AccountSettingsPage);
    qDebug() << Q_FUNC_INFO;

    if (d->accountPtr->isReady(d->accountFeatures)) {
        Tp::Avatar accountAvatar = d->accountPtr->avatar();
        d->avatarImageData = accountAvatar.avatarData;
        d->avatarImageType = accountAvatar.MIMEType;

        if (d->avatarImageData.isEmpty()) {
            qDebug() << "Got a NULL Avatar image";
            QMetaObject::invokeMethod(this, "requestAvatarData", Qt::QueuedConnection);
        } else {
            qDebug() << "Found  a proper image with type" << d->avatarImageType;
        }
        connect((Tp::Account *)d->accountPtr.data(), SIGNAL(avatarChanged(const Tp::Avatar &)),
                d, SLOT(onAvatarChange(const Tp::Avatar &)));
        if (!d->avatarImageData.isEmpty()) {
            const char *fmt = NULL;
            if (d->avatarImageType.contains("jpeg", Qt::CaseInsensitive)
                    || d->avatarImageType.contains("jpg", Qt::CaseInsensitive)) {
                qDebug() << "JPG format";
                fmt = "JPG";
            }

            QImage img;
            img.loadFromData(d->avatarImageData, fmt);
            d->saveImage(img);
            qDebug() << "Received  a proper image with type" << d->avatarImageType;
        } else {
            d->avatarItem->setImage("icon-m-telephony-avatar-placeholder");
            qDebug() << "Received a  NULL Avatar image";
        }

    } else {
        qDebug() << "cannot get avatar";
    }
}

void AccountSettingsPagePrivate::onAvatarChange(const Tp::Avatar &avatar)
{
    qDebug() << Q_FUNC_INFO;
    avatarImageData = avatar.avatarData;
    avatarImageType = avatar.MIMEType;

    if (!avatarImageData.isEmpty()) {
        const char *fmt = NULL;
        if (avatarImageType.contains("jpeg", Qt::CaseInsensitive) || avatarImageType.contains("jpg", Qt::CaseInsensitive)) {
            qDebug() << "JPG format";
            fmt = "JPG";
        }

        QImage img;
        img.loadFromData(avatarImageData, fmt);
        avatarItem->setImage(img);
        qDebug() << "Received  a proper image with type" << avatarImageType;
    } else {
        qDebug() << "Received a  NULL Avatar image";
    }
}

void AccountSettingsPagePrivate::onAccountManagerReady(Tp::PendingOperation *op)
{
    Q_Q(AccountSettingsPage);
    if (op->isError()) {
        qDebug() << "Account Manager cannot become ready";
        emit q->avatarInitFailed();
    } else {
        bool accFound = false;

        QList<Tp::AccountPtr> givenProviderAccounts = accountMgr->allAccounts();
        int numOfAccounts = givenProviderAccounts.count();
        if (numOfAccounts == 0) {
            qDebug() << "CANNOT GET EVEN ONE ACCOUNT";
            emit q->avatarInitFailed();
            return;
        }

        for (int i = 0; i < numOfAccounts; i++) {
            Tp::AccountPtr acc = givenProviderAccounts[i];
            qDebug() << "Retrieved account object path" << acc->objectPath() << "account bus name" << acc->busName()
                    << "Retrieved account display name" << acc->displayName()
                    << "Retrieved account nickname" << acc->nickname()
                    << "tmc Account Path" << accountPath;
            if (acc->objectPath() == accountPath) {
                qDebug() << "Found the right account";
                accountPtr = acc;
                accFound = true;
                break;
            }
        }

        bool isValid =  accountPtr->isValid();
        if (accFound && isValid) {
            connect(accountPtr->becomeReady(accountFeatures),
                    SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(accountReady(Tp::PendingOperation *)));
        }
        if (!isValid)
            qDebug() << "Account Not Valid";
    }
}

void AccountSettingsPagePrivate::accountReady(Tp::PendingOperation *op)
{
    Q_Q(AccountSettingsPage);
    if (op->isError()) {
        qDebug() << "Account could not become ready" << op->errorMessage() << op->errorName();
        emit q->avatarInitFailed();
        return;
    }

    qDebug() << "Account Ready";
    emit q->avatarInitCompleted();
}

void AccountSettingsPage::createPageActions()
{
    Q_D(AccountSettingsPage);
    MAction *action;

    //% "Save"
    action = new MAction(qtTrId("qtn_comm_save"), this);
    action->setLocation(MAction::ToolBarLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()),
            d, SLOT(saveSettings()));

    //% "Cancel"
    action = new MAction(qtTrId("qtn_comm_cancel"), this);
    action->setLocation(MAction::ToolBarLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()),
            ProviderPluginProcess::instance(), SLOT(quit()));

    // Hide the standard back/close button
    setComponentsDisplayMode(MApplicationPage::EscapeButton,
                             MApplicationPageModel::Hide);

    //% "Delete"
    action = new MAction(qtTrId("qtn_comm_command_delete"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()),
            this, SLOT(removeAccount()));
}

void AccountSettingsPage::createContent()
{
    Q_D(AccountSettingsPage);

    if (d->context == 0) return;

    //we need a central widget to get the right layout size under the menubar
    MWidget *centralWidget = new MWidget();
    d->layout = new MLayout(centralWidget);
    d->layout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *layoutPolicy =
        new MLinearLayoutPolicy(d->layout, Qt::Vertical);
    layoutPolicy->setSpacing(0);

    MLayout *layoutPanel = new MLayout(d->panel);
    layoutPanel->setContentsMargins(0, 0, 0, 0);
    d->panelPolicy = new MLinearLayoutPolicy(layoutPanel, Qt::Vertical);

    QGraphicsLayoutItem *accountSettingsLayout = createAccountSettingsLayout();
    layoutPolicy->addItem(accountSettingsLayout);

    QGraphicsLayoutItem *serviceWidget = createServiceSettingsLayout();
    layoutPolicy->addItem(serviceWidget);

    layoutPolicy->addStretch();

    setCentralWidget(centralWidget);

    createPageActions();
    QString path;
    if (d->avatar.text() == "true") {
        Tp::registerTypes();
        Tp::enableDebug(false);
        Tp::enableWarnings(true);
        foreach (AbstractServiceSetupContext *context, d->contexts) {
            d->abstractContexts.append(context);
            const Accounts::Service *service = context->service();
            d->account->selectService(service);
            d->accountPath = d->account->valueAsString("tmc-uid");
            if (path.isNull()) {
                path = d->accountPath;
            }
        }
        if (!path.contains("/org/freedesktop/Telepathy/Account/"))
            path.prepend("/org/freedesktop/Telepathy/Account/");
        d->accountPath = path;
        d->accountMgr = Tp::AccountManager::create();
        if (!d->accountMgr.isNull()) {
            connect((d->accountMgr->becomeReady(Tp::AccountManager::FeatureCore)),
                    SIGNAL(finished(Tp::PendingOperation *)), d,
                    SLOT(onAccountManagerReady(Tp::PendingOperation *)));

            QSet<Tp::Feature> featuresSet;
            featuresSet << Tp::Account::FeatureAvatar;
            d->accountFeatures = Tp::Features(featuresSet);
            connect(this, SIGNAL(avatarInitCompleted()), this, SLOT(getAvatar()));
            connect(this, SIGNAL(avatarInitFailed()), this, SLOT(initFailed()));
        }
    }
}

const AbstractAccountSetupContext *AccountSettingsPage::context()
{
    Q_D(AccountSettingsPage);
    return d->context;
}

void AccountSettingsPage::enable(bool state)
{
    Q_D(AccountSettingsPage);
    d->panel->setEnabled(state);
}

void AccountSettingsPage::removeAccount()
{
    Q_D(AccountSettingsPage);
    //% "Delete %1 from your device?"
    QString dialogTitle =
        qtTrId("qtn_acc_remove_account").arg(d->context->account()->displayName());
    //% "All content related to this account will be deleted permanently"
    MMessageBox removeMBox(dialogTitle, qtTrId("qtn_acc_remove_account_statement"),
                           M::YesButton | M::NoButton);
    removeMBox.setStyleName("RemoveDialog");

    if (removeMBox.exec() == M::YesButton) {
        d->context->account()->remove();
        d->context->account()->sync();
        ProviderPluginProcess::instance()->quit();
    }
}

void AccountSettingsPage::setWidget(MWidget *widget)
{
     Q_D(AccountSettingsPage);
     d->panelPolicy->addItem(widget);
}

void AccountSettingsPage::setHiddenServices(const Accounts::ServiceList &hiddenServices)
{
    Q_D(AccountSettingsPage);
    d->hiddenServiceList = hiddenServices;
}

MButton *AccountSettingsPage::enableAccountButton() const
{
    Q_D(const AccountSettingsPage);
    return d->enableButton;
}

AccountSyncHandler *AccountSettingsPage::accountSyncHandler() const
{
    Q_D(const AccountSettingsPage);
    return d->syncHandler;
}

void AccountSettingsPage::changeAvatar()
{
    Q_D(const AccountSettingsPage);
    qDebug() << Q_FUNC_INFO;
    d->avatarSelector->init();
    connect(d->avatarSelector, SIGNAL(avatarSelected(const QImage &)),
            d, SLOT(onAvatarSelectedFromGallery(const QImage &)));
    d->avatarSelector->launch();
}

void AccountSettingsPagePrivate::onAvatarSelectedFromGallery(const QImage &image)
{
    qDebug() << Q_FUNC_INFO;
    disconnect(avatarSelector, SIGNAL(avatarSelected(const QImage &)),
               this, SLOT(onAvatarSelectedFromGallery(const QImage &)));
    saveImage(image);
}

void AccountSettingsPagePrivate::saveImage(const QImage &image)
{
    qDebug() << Q_FUNC_INFO;

    if (!image.isNull()) {

        qDebug() << "Received image" << image.width() << image.height();
        QImage scaledImg;
        scaleImage(image, scaledImg);

        if (!scaledImg.isNull()) {
            setAvatarImage(scaledImg);
            QByteArray rawData;
            QBuffer buff(&rawData);
            buff.open(QIODevice::WriteOnly);
            if (scaledImg.save(&buff, "JPG")) {
                qDebug() << "Writing to ByteArray succesful" << rawData.size();
                avatarImageData = rawData;
                avatarImageType = "image/jpeg";
            }
        }
    } else {
        qDebug() << "NULL image from Gallery";
    }
}
