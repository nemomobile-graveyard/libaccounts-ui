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

#ifndef ACCOUNTSUI_ABSTRACT_ACCOUNT_SETUP_CONTEXT_H
#define ACCOUNTSUI_ABSTRACT_ACCOUNT_SETUP_CONTEXT_H

//AccountsUI
#include <AccountsUI/abstract-setup-context.h>

//Meegotouch
#include <MWidget>

namespace AccountsUI {

/*!
 * Operation type.
 */
enum SetupType {
    CreateNew,
    EditExisting,
};

class AbstractAccountSetupContextPrivate;

/*!
 * @class AbstractAccountSetupContext
 * @headerfile AccountsUI/abstract-account-setup-context.h \
 * AccountsUI/AbstractAccountSetupContext
 * @brief Context object for provider plugins.
 */
class ACCOUNTSUI_EXPORT AbstractAccountSetupContext: public AbstractSetupContext
{
    Q_OBJECT

public:
    /*!
     * Constructor.
     */
    AbstractAccountSetupContext(Accounts::Account *account, SetupType type,
                                QObject *parent = 0);
    virtual ~AbstractAccountSetupContext();

    /*!
     * @return The SetupType specified when creating the context.
     */
    SetupType setupType() const;

#ifndef ACCOUNTSUI_DISABLE_DEPRECATED
    /*!
     * @deprecated
     */
    SetupType setupType();
#endif

    /*!
     * Constructs the UI element which handles account creation/settings/deletion.
     */
    virtual MWidget *widget(QGraphicsItem *parent = 0) = 0;

    /*!
     * Set the service type.
     */
    void setServiceType(const QString &serviceType);

    /*!
     * @return The service type.
     */
    const QString serviceType() const;

    /*!
     * This method returns the username currently set in the UI. It's meant to
     * be called by service plugins, during the validation phase, if they need
     * to store the username in their service settings.
     *
     * This function returns a non-empty string only if setUserName() has
     * previously been called by the account plugin itself.
     *
     * @return The account username.
     */
    QString userName() const;

protected:
    /*!
     * The account setup context implementation should call this method before
     * starting the validation of the service contexts. It's used so that
     * service plugins could get the username, if they need to.
     *
     * @param userName The user name currently written in the UI.
     */
    void setUserName(const QString &userName);

private:
    AbstractAccountSetupContextPrivate *d_ptr;
    Q_DECLARE_PRIVATE(AbstractAccountSetupContext)
};

} // namespace

#endif // ACCOUNTSUI_ABSTRACT_ACCOUNT_SETUP_CONTEXT_H
