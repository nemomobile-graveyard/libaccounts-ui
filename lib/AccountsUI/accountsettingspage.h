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

#ifndef ACCOUNTSUI_ACCOUNTSETTINGSPAGE_H
#define ACCOUNTSUI_ACCOUNTSETTINGSPAGE_H

//libAccountsUI
#include <AccountsUI/abstract-account-setup-context.h>
#include <AccountsUI/account-sync-handler.h>
#include <AccountsUI/service-helper.h>
#include <AccountsUI/common.h>

//Meegotouch
#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MWidget>

namespace Accounts {
    class Account;
}

namespace AccountsUI {

class AbstractAccountSetupContext;
class AccountSettingsPagePrivate;

/*!
 * @class AccountSettingsPage
 * @headerfile AccountsUI/accountsettingspage.h AccountsUI/AccountSettingsPage
 * @brief Main UI element for editing an existing account.
 *
 * @details The AccountSettingsPage is the page that appears when the user
 * clicks on an existing account to edit its settings. It is created by
 * the ProviderPluginProcess::mainPage() method, right before starting the
 * execution of the MApplication.
 */
class ACCOUNTSUI_EXPORT AccountSettingsPage: public MApplicationPage
{
    Q_OBJECT

public:
    /*!
     * Class constructor.
     */
    AccountSettingsPage(AccountsUI::AbstractAccountSetupContext *context);
    ~AccountSettingsPage();

    /*!
     * Create the page contents.
     */
    virtual void createContent();

    /*!
     * Creates the layout of all the services as per serviceType
     */
    void setServicesToBeShown();

    /*!
     * Returns the context.
     */
    const AbstractAccountSetupContext *context();


    void setWidget(MWidget *widget);

    /*!
     * Allows clients to hide services from service selection screen.
     */
    void setHiddenServices(const Accounts::ServiceList &hiddenServices);

public slots:
    /*!
     * Called when the user toggles the enable button for the account.
     * The default implementation changes the status of the account and
     * if necessary updates the UI widget to reflect the new state.
     */
    virtual void enable(bool state);

    /*!
     * Called when the user triggers the action of deleting the account.
     * The default implementation deletes the account, after asking
     * confirmation to the user.
     */
    virtual void removeAccount();

Q_SIGNALS:
    void serviceEnabled(const QString &serviceName, bool enabled);

private slots:
    void saveSettings();
    void openChangePasswordDialog();
    void onSyncStateChanged(const SyncState &state);
    void deleteCredentialsDialog();
    void disableSameServiceTypes(const QString &serviceType);

private:
    AccountSettingsPagePrivate* d_ptr;
    Q_DISABLE_COPY(AccountSettingsPage)
    Q_DECLARE_PRIVATE(AccountSettingsPage)

};

} //namespace

#endif // ACCOUNTSUI_ACCOUNTSETTINGSPAGE_H
