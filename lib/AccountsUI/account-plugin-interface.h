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

#ifndef ACCOUNTSUI_ACCOUNT_PLUGIN_INTERFACE_H
#define ACCOUNTSUI_ACCOUNT_PLUGIN_INTERFACE_H

//AccountsUI
#include <AccountsUI/AbstractAccountSetupContext>

//Qt
#include <QtPlugin>
#include <QDomDocument>

namespace AccountsUI {

/*!
 * @class AccountPluginInterface
 * @deprecated The AccountPluginInterface class has been deprecated and will be removed in
 * future libraries
 * @brief Interface for account plugins
 *
 * @details The AccountPluginInterface is to be implemented by provider
 * plugins which will be loaded by the account adding application. They must
 * provide functionality to allow the user to create and add an account for the given
 * provider.
 *
 * The accounts UI will look for all the files
 * /usr/share/accounts/providers/&lt;name&gt;.provider to get the list of
 * available providers.
 * Examples of &lt;name&gt;: google, ovi, yahoo, jabber, openID
 *
 * The .provider file is an XML file which contains at least the following information:
 *
 * - the name of the plugin to be used to configure this account: if name is
 *   &lt;name&gt;, accounts UI will look for a plugin in
 *   /usr/lib/account/providers/lib&lt;name&gt;.so
 *   Examples: google, ovi, yahoo
 *
 * - the "pretty" name of the provider. If the XML element has the "dynamic"
 *   attribute set, then the name must be retrieved by first loading the
 *   plugin and invoking its displayName() method.
 *   Examples: Google, OVI, Yahoo!
 *
 * - icon name. If the XML element has the "dynamic" attribute set, then the
 *   icon widget must be obtained by loading the plugin and invoking the
 *   iconWidget() method.
 *   Examples: icon_provider_google, icon_provider_ovi.
 */

class ACCOUNTSUI_EXPORT AccountPluginInterface
{
public:
    virtual ~AccountPluginInterface() {}

    /*!
     * Get the provider's icon as a widget.
     * @param domDocument The DOM for the account provider XML file.
     * @param parent The parent object.
     */
    virtual MWidget *iconWidget(const QDomDocument &domDocument,
                                QGraphicsItem *parent = 0) = 0;

    /*!
     * Get the provider's name as a QString
     * @param domDocument The DOM for the account provider XML file.
     */
    virtual QString prettyName(const QDomDocument &domDocument) = 0;

    /*!
     * Get the provider's description as string
     * @param domDocument The DOM for the account provider XML file.
     */
    virtual QString description(const QDomDocument &domDocument) = 0;

    /*!
     * Gets the context object which handles account
     * creation/settings/deletion.
     *
     * @param account the account which is used
     * @param type is the requested page type which is needed.
     * @param parent The parent object for the context.
     */
    virtual AbstractAccountSetupContext *accountSetupContext(Accounts::Account *account,
                                                             SetupType type,
                                                             QObject *parent = 0) = 0;
};

} // namespace

Q_DECLARE_INTERFACE(AccountsUI::AccountPluginInterface,
                    "com.nokia.AccountsUI.AccountPluginInterface/1.0")

#endif // ACCOUNTSUI_ACCOUNT_PLUGIN_INTERFACE_H
