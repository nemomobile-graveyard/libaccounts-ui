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

#ifndef ACCOUNTSUI_ABSTRACT_SETUP_CONTEXT_H
#define ACCOUNTSUI_ABSTRACT_SETUP_CONTEXT_H

#include <AccountsUI/common.h>

//accounts-qt
#include <Accounts/Account>

//Qt
#include <QObject>

//Meegotouch
#include <MWidget>

namespace AccountsUI {

enum ErrorCode {
    UnknownError,
};

class AbstractSetupContextPrivate;

/*!
 * @class AbstractSetupContext
 * @headerfile AccountsUI/abstract-setup-context.h \
 * AccountsUI/AbstractSetupContext
 * @brief Abstract context for accont plugins
 *
 * This is the base class for provider and service plugins.
 */
class ACCOUNTSUI_EXPORT AbstractSetupContext : public QObject
{
    Q_OBJECT

public:
    /*!
     * Class constructor
     */
    explicit AbstractSetupContext(Accounts::Account *account,
                                  QObject *parent = 0);
    virtual ~AbstractSetupContext();

    /*!
     * Gets the account being setup by this context.
     */
    Accounts::Account *account();

public Q_SLOTS:
    /*!
     * Stores the changes into the account.
     *
     * This method is called by the Accounts UI before calling
     * Account::sync(). The plugin should store all the settings into the
     * account, but it must not call Account::sync() itself.
     *
     * The account plugin might also perform other actions which are needed to
     * complete the account setup.
     *
     * Once the store() is completed, the account plugin must emit either the
     * stored() or the error() signal.
     */
    virtual void store() = 0;

    /*!
     * Aborts the setup operation.
     *
     * This method is called by the Accounts UI if the setup operation needs
     * to be aborted. If the plugin only stored data into the Account object,
     * then no action needs to be performed here. But all other changes should
     * be reverted.
     */
    virtual void abort();

    /*!
     * Validates the given credentials for the account logins
     */
    virtual void validate();

Q_SIGNALS:
    /*!
     * Emitted in response to a store() request. This signal tells the
     * Accounts UI that the plugin has stored all the settings into the
     * account, and that the Accounts Ui can now call the Account::sync()
     * method.
     */
    void stored();

    /*!
     * Emitted when some error occurs.
     */
    void error(AccountsUI::ErrorCode code, const QString &message);

    /*!
     * Signals that validation succeeded
     */
    void validated();

    /*!
     * Emitted when validating process started. Emitting this signal is
     * not mandatory but when emitted the UI might indicate to user that
     * the process of validating is running. The signal should be emitted
     * in reimplementation of AbstractSetupContext::validate() method.
     */
    void validating();

private:
    AbstractSetupContextPrivate *d_ptr;
    Q_DECLARE_PRIVATE(AbstractSetupContext)
};

} // namespace

#endif // ACCOUNTSUI_ABSTRACT_SETUP_CONTEXT_H
