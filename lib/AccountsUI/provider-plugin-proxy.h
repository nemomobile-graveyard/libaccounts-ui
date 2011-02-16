/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2010 Nokia Corporation.
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

#ifndef ACCOUNTSUI_PROVIDER_PLUGIN_PROXY_H
#define ACCOUNTSUI_PROVIDER_PLUGIN_PROXY_H

//libAccountsUI
#include <AccountsUI/common.h>

//Accounts
#include <Accounts/Account>
#include <Accounts/Provider>

//Qt
#include <QProcess>

namespace AccountsUI {

class LastPageActions;
class ProviderPluginProxyPrivate;

/*!
 * @class ProviderPluginProxy
 * @headerfile AccountsUI/provider-plugin-proxy.h \
 * AccountsUI/ProviderPluginProxy
 * @brief Client class for accounts UI plugins.
 *
 * @details The ProviderPluginProxy class can be used to run the account
 * plugins. Plugins can be started with the createAccount() and editAccount()
 * methods, respectively to enter the account creation and editing modes.
 * Plugin lifetime can be monitored with the created(), edited(), cancelled()
 * signals, or inspected with the isPluginRunning() method.
 */
class ACCOUNTSUI_EXPORT ProviderPluginProxy : public QObject
{
    Q_OBJECT

public:
    /*!
     * Constructor
     */
    ProviderPluginProxy(QObject *parent = 0);
    virtual ~ProviderPluginProxy();

    /*!
     * Runs the account plugin to create an account.
     *
     * @param provider The Accounts::Provider for the account to be created.
     * @param serviceType The main service type the user is interested in, or
     * empty string.
     */
    void createAccount(Accounts::Provider *provider,
                       const QString &serviceType);
    /*!
     * Runs the account plugin to create an account.
     *
     * @param provider The Accounts::Provider for the account to be created.
     * @param serviceType The main service type the user is interested in, or
     * empty string.
     * @param lastPageActions Structure describing what should be offered to
     * the user in the last page of a successful account creation.
     */
    void createAccount(Accounts::Provider *provider,
                       const QString &serviceType,
                       const LastPageActions &lastPageActions);

    /*!
     * Runs the account plugin to edit an account.
     *
     * @param account The Accounts::Account to be edited.
     * @param serviceType The main service type the user is interested in, or
     * empty string.
     */
    void editAccount(Accounts::Account *account, const QString &serviceType);

    /*!
     * Checks whether a plugin is running.
     *
     * @return Returns true if there is plugin process running.
     */
    bool isPluginRunning();

    /*!
     * @return the name of the currently running plugin, or empty string if no
     * plugin is running.
     */
    QString pluginName();

    /*!
     * @return the name of the povider of the plugin
     */
    QString providerName();

public Q_SLOTS:
    /*!
     * Stops the plugin process.
     */
    void stopProcess();

Q_SIGNALS:
    void created(int accountId);
    void edited();
    void failed();

private:
    ProviderPluginProxyPrivate *d_ptr;
    Q_DECLARE_PRIVATE(ProviderPluginProxy)
};

} // namespace

#endif // ACCOUNTSUI_PROVIDER_PLUGIN_PROXY_H

