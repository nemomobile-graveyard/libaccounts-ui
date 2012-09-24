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

#ifndef ACCOUNTSUI_SERVICE_PLUGIN_INTERFACE_H
#define ACCOUNTSUI_SERVICE_PLUGIN_INTERFACE_H

//libAccountsUI
#include <AccountsUI/abstract-service-setup-context.h>

//Qt
#include <QtPlugin>

namespace AccountsUI {

/*! @class ServicePluginInterface
 *
 *  @brief Interface for account service plugins
 *
 *  @details The ServicePluginInterface is to be implemented by service
 *  plugins which will be loaded by the account editing application. They must
 *  provide functionality to allow the user to edit the service settings for
 *  the selected account.
 */
class ACCOUNTSUI_EXPORT ServicePluginInterface
{
public:
    virtual ~ServicePluginInterface() {}

    /*!
     * Get the service's icon as a widget.
     * @param domDocument The DOM for the account service XML file.
     * @param parent A parent widget for the icon.
     */
    virtual MWidget *iconWidget(const QDomDocument &domDocument,
                                QGraphicsItem *parent = 0) = 0;

    /*!
     * Gets context object which handles the service settings.
     *
     * @param account The Account being edited.
     * @param service The account service.
     * @param parent The parent object for the context.
     */
    virtual AbstractServiceSetupContext *serviceSetupContext(
        Accounts::Account *account,
        const Accounts::Service &service,
        QObject *parent = 0) = 0;
};

} // namespace

Q_DECLARE_INTERFACE(AccountsUI::ServicePluginInterface,
                    "com.nokia.AccountsUI.ServicePluginInterface/1.0")

#endif // ACCOUNTSUI_SERVICE_PLUGIN_INTERFACE_H
