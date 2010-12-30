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

#ifndef ACCOUNTSUI_LAST_PAGE_ACTIONS_H
#define ACCOUNTSUI_LAST_PAGE_ACTIONS_H

//libAccountsUI
#include <AccountsUI/common.h>

//Qt
#include <QList>
#include <QString>

namespace AccountsUI {

/*!
 * @class LastPageActions
 * @headerfile AccountsUI/last-page-actions.h \
 * AccountsUI/LastPageActions
 * @brief Specify user actions to appear at the end of an account creation
 *
 * @details This class is used in the ProviderPluginProxy class to specify
 * what actions are available to the user at the end of the account creation
 * procedure.
 *
 * @note Do not subclass this class! Its size might change in future versions.
 */
class ACCOUNTSUI_EXPORT LastPageActions
{
public:
    class ACCOUNTSUI_EXPORT ServiceAction {
    public:
        ServiceAction(const QString &title, const QString &serviceName):
            m_title(title),
            m_serviceName(serviceName)
        {}
        ~ServiceAction() {}

        QString title() const { return m_title; }
        QString serviceName() const { return m_serviceName; }

        void execute() const;
    private:
        QString m_title;
        QString m_serviceName;
    };

    typedef QList<ServiceAction> ServiceActionList;
    /*!
     * Constructor
     */
    LastPageActions() {};
    virtual ~LastPageActions() {};

    /*!
     * Add an action which consists of invoking an application registered in
     * the MeeGoTouch service FW. The application will be executed by calling
     * MApplicationIfProxy::launch() method.
     * @param title The action name, as shown to the user.
     * @param serviceName The name of the service to launch.
     */
    void addServiceAction(const QString &title, const QString &serviceName)
    {
        m_serviceActions.append(ServiceAction(title, serviceName));
    }

    const ServiceActionList serviceActions() const { return m_serviceActions; }

    static void executeService(const QString &serviceName);

private:
    ServiceActionList m_serviceActions;
};

} // namespace

#endif // ACCOUNTSUI_LAST_PAGE_ACTIONS_H

