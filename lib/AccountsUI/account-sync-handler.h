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

#ifndef ACCOUNTSUI_ACCOUNT_SYNC_HANDLER_H
#define ACCOUNTSUI_ACCOUNT_SYNC_HANDLER_H

//libAccountsUI
#include <AccountsUI/abstract-account-setup-context.h>
#include <AccountsUI/abstract-service-setup-context.h>
#include <AccountsUI/common.h>

namespace AccountsUI {

    /*!
     * State of validating and storing process which is passed through
     * signal finished(const FinishedMessage).
     */
    enum SyncState {
        Validated,
        NotValidated,
        Stored,
        NotStored
    };

class AccountSyncHandlerPrivate;

/*!
 * @class AccountSyncHandler
 * @brief Handling the account sync
 *
 * @details The class is handling process of synchronization of newly created
 * account and its services. It will validate and store the the account and
 * its services settings.
 * After emiting syncStateChanged(Stored) the caller should sync the account.
 */
class ACCOUNTSUI_EXPORT AccountSyncHandler : public QObject
{
    Q_OBJECT

public:
     /*!
     * Class constructor
     */
    AccountSyncHandler(QObject *parent = 0);
    /*!
     * Destructor
     */
    virtual ~AccountSyncHandler();


    /*!
     * Validate context.
     */
    void validate(QList<AbstractSetupContext*> contexts);

    /*!
     * Store context.
     */
    void store(QList<AbstractSetupContext*> contexts);

signals:
    /*!
     * Emits when process of validating and storing has changed the state
     * @param Message of current state.
     */
    void syncStateChanged(const SyncState&);

private slots:
    void onContextValidated();
    void onContextValidateError(AccountsUI::ErrorCode code,
                                const QString &message);
    void onContextStored();
    void onContextError(AccountsUI::ErrorCode code,
                        const QString &message);

private:
    AccountSyncHandlerPrivate *d_ptr;
    Q_DISABLE_COPY(AccountSyncHandler)
    Q_DECLARE_PRIVATE(AccountSyncHandler)
    void releaseValidateLock(AbstractSetupContext *context = 0);
    void releaseStoreLock(AbstractSetupContext *context = 0);
    void storeSettings();
};

} // namespace

#endif // ACCOUNTSUI_ABSTRACT_SERVICE_SETUP_CONTEXT_H
