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


namespace AccountsUI {

class AccountSettingsPagePrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(AccountSettingsPage)

public:
    AccountSettingsPagePrivate(AbstractAccountSetupContext *context);
    ~AccountSettingsPagePrivate() {}

public Q_SLOTS:
    void saveSettings();
    void openChangePasswordDialog();
    void onSyncStateChanged(const SyncState &state);
    void deleteCredentialsDialog();
    void disableSameServiceTypes(const QString &serviceType);
    void setEnabledService(const QString& serviceName, bool enabled);

private:
    mutable AccountSettingsPage *q_ptr;
    AbstractAccountSetupContext *context;
    Accounts::Account *account;
    QList<AbstractServiceSetupContext *> contexts;
    MDetailedListItem *usernameAndStatus;
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
};

} // namespace

#endif // ACCOUNTSUI_ACCOUNT_SETTINGS_PAGE_PRIV_H
