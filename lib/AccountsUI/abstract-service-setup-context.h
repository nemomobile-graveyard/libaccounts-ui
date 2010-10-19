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

#ifndef ACCOUNTSUI_ABSTRACT_SERVICE_SETUP_CONTEXT_H
#define ACCOUNTSUI_ABSTRACT_SERVICE_SETUP_CONTEXT_H

//AccountsUI
#include <AccountsUI/abstract-setup-context.h>

//Meegotouch
#include <MWidget>

namespace AccountsUI {

class AbstractAccountSetupContext;
class AbstractServiceSetupContextPrivate;

/*!
 * @class AbstractServiceSetupContext
 * @headerfile AccountsUI/abstract-service-setup-context.h \
 * AccountsUI/AbstractServiceSetupContext
 * @brief Context object for service plugins.
 */
class ACCOUNTSUI_EXPORT AbstractServiceSetupContext: public AbstractSetupContext
{
    Q_OBJECT

public:
    /*!
     * Class constructor
     */
    AbstractServiceSetupContext(Accounts::Account *account,
                                const Accounts::Service *service,
                                QObject *parent = 0);
    virtual ~AbstractServiceSetupContext();

    /*!
     * Gets the service being setup by this context.
     */
    const Accounts::Service *service() const;

    /*!
     * Sets the AbstractServiceSetupContext. This method should not be called
     * by the service plugin itself.
     * @param context The AbstractAccountSetupContext.
     */
    void setAccountSetupContext(AbstractAccountSetupContext *context);

    /*!
     * Gets the AbstractAccountSetupContext.
     * @return The AbstractAccountSetupContext which was set with
     * setAccountSetupContext(), or 0 if none was set.
     */
    AbstractAccountSetupContext *accountSetupContext() const;

    /*!
     * Constructs the UI element which handles account
     * creation/settings/deletion.
     * @param parent The parent widget.
     * @param showAllSettings Whether all the settings should be shown, or
     * only the mandatory ones.
     */
    virtual MWidget *widget(QGraphicsItem *parent = 0,
                            bool showAllSettings = true) = 0;

    /*!
     * Checks whether the service has mandatory settings.
     *
     * The base implementation of this method returns true.
     */
    virtual bool hasMandatorySettings() const;

    /*!
     * Enables/disables the service for the selected account.
     *
     * The base implementation of this method selects the service on the
     * account and then calls account->setEnabled().
     *
     * @param enabled Whether the service should be enabled.
     */
public slots:
    virtual void enable(bool enabled);

private:
    AbstractServiceSetupContextPrivate *d_ptr;
    Q_DECLARE_PRIVATE(AbstractServiceSetupContext)
};

} // namespace

#endif // ACCOUNTSUI_ABSTRACT_SERVICE_SETUP_CONTEXT_H
