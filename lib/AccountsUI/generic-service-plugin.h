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

#ifndef ACCOUNTSUI_GENERICSERVICEPLUGIN_H
#define ACCOUNTSUI_GENERICSERVICEPLUGIN_H

//M
#include <MWidget>

//libAccountsUI
#include <AccountsUI/service-plugin-interface.h>

namespace AccountsUI {

class AbstractServiceSetupContext;

/*!
 * @class GenericServicePlugin
 * @headerfile AccountsUI/generic-service-plugin.h \
 * AccountsUI/GenericServicePlugin
 * @brief Default implementation of a service plugin.
 *
 * The GenericServicePlugin is the default implementation of service
 * plugins; it is automatically used when the service only consists of an
 * XML file.
 */
class ACCOUNTSUI_EXPORT GenericServicePlugin :
    public AccountsUI::ServicePluginInterface
{
    Q_INTERFACES(AccountsUI::ServicePluginInterface)

public:
    /*!
     * Returns the service icon.
     * @param domDocument The XML service document.
     * @param parent A parent object for the icon widget.
     */
    virtual MWidget *iconWidget(const QDomDocument &domDocument,
                                QGraphicsItem *parent = 0);

    /*!
     * Creates the setup context.
     * @param account The account being edited.
     * @param service The service.
     * @param parent A parent object for the context.
     *
     * This is a virtual method. The default implementation returns a
     * GenericServiceSetupContext.
     */
    virtual AccountsUI::AbstractServiceSetupContext *serviceSetupContext(
        Accounts::Account *account,
        const Accounts::Service *service,
        QObject *parent = 0);
};

} // namespace

#endif // ACCOUNTSUI_GENERICSERVICEPLUGIN_H
