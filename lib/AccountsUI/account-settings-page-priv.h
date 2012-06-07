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

#ifndef ACCOUNTSUI_ACCOUNT_SETTINGS_PAGE_PRIV_H
#define ACCOUNTSUI_ACCOUNT_SETTINGS_PAGE_PRIV_H

//project
#include "account-settings-page.h"
#include "service-settings-widget.h"
#include "account-sync-handler.h"

//Accounts
#include <Accounts/Account>

//Telepathy
#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/ReferencedHandles>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Connection>

//Meegotouch
#include <MButton>
#include <MDetailedListItem>
#include <MLayout>
#include <MLinearLayoutPolicy>

//Qt
#include <QMultiMap>

//Accounts-Ui
#include "abstract-account-setup-context.h"
#include "accountsmanagersingleton.h"
#include "avatar-selector.h"


namespace AccountsUI {

class AvatarListItem : public MBasicListItem
{
    Q_OBJECT

public:
    AvatarListItem(QGraphicsWidget *parent = 0);
    ~AvatarListItem();
    QGraphicsLayout *createLayout();
    void setImage(const QImage &image);
    void setImage(const QString &id);

private:
    MLayout *horizontalLayout;
    MImageWidget *avatarImage;
};

class AvatarSelector;
class AccountSettingsPagePrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(AccountSettingsPage)

public:

    static const int avatar_width_small = 64;
    static const int avatar_height_small = 64;
    static const int avatar_width_medium = 128;
    static const int avatar_height_medium = 128;
    static const int avatar_width_large = 256;
    static const int avatar_height_large = 256;

    AccountSettingsPagePrivate(AbstractAccountSetupContext *context);
    ~AccountSettingsPagePrivate() {}

    bool hasSingleService() const;
    void scaleImage(const QImage &image, QImage &scaledImage);
    void setAvatarImage(const QImage &scaledImg);
    void saveImage(const QImage &image);

public Q_SLOTS:
    void saveSettings();
    void openChangePasswordDialog();
    void onSyncStateChanged(const SyncState &state);
    void deleteCredentialsDialog();
    void disableSameServiceTypes(const QString &serviceType);
    void setEnabledService(const QString& serviceName, bool enabled);
    void finishedCalled(Tp::PendingOperation *op);
    void onAvatarSelectedFromGallery(const QImage &image);
    void onAccountManagerReady(Tp::PendingOperation *op);
    void accountReady(Tp::PendingOperation *op);
    void onAvatarChange(const Tp::Avatar &avatar);

private:
    mutable AccountSettingsPage *q_ptr;
    AbstractAccountSetupContext *context;
    Accounts::Account *account;
    QList<AbstractServiceSetupContext *> contexts;
    MButton *enableButton;
    Accounts::ServiceList serviceList;
    QList<AbstractSetupContext*> abstractContexts;
    AccountSyncHandler *syncHandler;
    bool changePasswordDialogStarted;
    QMultiMap<QString, ServiceSettingsWidget*> settingsWidgets;
    MWidgetController *panel;
    MLayout *layout;
    MLinearLayoutPolicy *panelPolicy;
    Accounts::ServiceList hiddenServiceList;
    QMap<QString, bool> serviceStatusMap;
    bool saving;
    QString accountPath;
    AvatarSelector *avatarSelector;
    AvatarListItem *avatarItem;
    int preferredWidth;
    int preferredHeight;
    Tp::Features accountFeatures;
    Tp::AccountPtr accountPtr;
    Tp::AccountManagerPtr accountMgr;
    Tp::ConnectionPtr connection;
    Tp::Features connectionFeatures;
    QByteArray avatarImageData;
    QString avatarImageType;
    QString connectionError;
    QDomElement avatar;
    Tp::PendingOperation *op;
};

} // namespace

#endif // ACCOUNTSUI_ACCOUNT_SETTINGS_PAGE_PRIV_H
