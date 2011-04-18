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

#ifndef ACCOUNTSUI_SERVICE_H
#define ACCOUNTSUI_SERVICE_H

//libAccountsUI
#include <AccountsUI/abstract-service-setup-context.h>

//Accounts
#include <Accounts/Service>

namespace AccountsUI {

class ServiceHelperPrivate;

/*!
 * @class ServiceHelper
 * @brief Helper class to handle service specific data and UIs
 *
 * @details The ServiceHelper class offers some methods to retrieve
 * service-specific data and takes care of loading the account service
 * plugins when necessary.
 */
class ACCOUNTSUI_EXPORT ServiceHelper : public QObject
{
    Q_OBJECT

public:
    /*!
     * Class constructor.
     */
    explicit ServiceHelper(Accounts::Service *service, QObject *parent = 0);
    virtual ~ServiceHelper();

    /*!
     * Gets the Accounts::Service that was used to construct this object.
     */
    Accounts::Service *service() const;

    /*!
     * Gets the QDomDocument installed by the service.
     */
    const QDomDocument domDocument() const;

    /*!
     * Gets an icon for the service. If the icon is dynamic, calling this
     * method will cause the loading of the service plugin.
     */
    MWidget *iconWidget(QGraphicsItem *parent = 0);

    /*!
     * Gets the display name of the service. This might cause the loading of the
     * service plugin.
     */
    QString prettyName();

    /*!
     * Gets the service short description. This might cause the loading of the
     * service plugin.
     */
    QString shortDescription();

    /*!
     * Gets the service description. This might cause the loading of the
     * service plugin.
     */
    QString description();

    /*!
     * Gets the service provider name.
     */
    QString providerName();

    /*!
     * Gets the service type
     */
    QString serviceType();

    /*!
     * Gets the service name
     */
    QString serviceName();

    /*!
     * Gets the context object which handles account service
     * creation/settings/deletion.
     * Calling this method will cause the loading of the service plugin.
     *
     * @param context The account setup context.
     * @param parent The parent object for the context.
     */
    AbstractServiceSetupContext *serviceSetupContext(
        AbstractAccountSetupContext *context,
        QObject *parent = 0);

#ifndef ACCOUNTSUI_DISABLE_DEPRECATED
    /*!
     * @deprecated
     * Gets the context object which handles account service
     * creation/settings/deletion.
     * Calling this method will cause the loading of the service plugin.
     *
     * @param account the account which is used
     * @param parent The parent object for the context.
     */
    AbstractServiceSetupContext *serviceSetupContext(
        Accounts::Account *account,
        QObject *parent = 0);
#endif

private:
    ServiceHelperPrivate *d_ptr;
    Q_DECLARE_PRIVATE(ServiceHelper)
};

} // namespace

#endif // ACCOUNTSUI_SERVICE_H
