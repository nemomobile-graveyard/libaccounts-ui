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

#include "account-sync-handler.h"
//Qt
#include <QDebug>

namespace AccountsUI {

class AccountSyncHandlerPrivate
{
public:
    AccountSyncHandlerPrivate()
        : validateLocks(0),
        storeLocks(0),
        validateError(false),
        storeError(false)
    {}
    ~AccountSyncHandlerPrivate() {}

    int validateLocks;
    int storeLocks;
    bool validateError;
    bool storeError;
};

AccountSyncHandler::AccountSyncHandler( QObject *parent)
        : QObject(parent),
        d_ptr(new AccountSyncHandlerPrivate())
{
}

AccountSyncHandler::~AccountSyncHandler()
{
    delete d_ptr;
}

void AccountSyncHandler::releaseValidateLock(AbstractSetupContext *context)
{
    qDebug() << Q_FUNC_INFO;
    Q_D(AccountSyncHandler);

    if (context != 0) {
        disconnect(context, SIGNAL(validated()),
                   this, SLOT(onContextValidated()));
        disconnect(context, SIGNAL(error(AccountsUI::ErrorCode, const QString &)),
                   this, SLOT(onContextValidateError(AccountsUI::ErrorCode, const QString &)));
    }
    Q_ASSERT(d->validateLocks > 0);
    d->validateLocks--;

    if (d->validateLocks == 0) {
        qDebug() << "All contexts validation replied";

        /* All contexts have replied.
         * Validated if no errors were reported */
        if (!d->validateError) {
            emit syncStateChanged(Validated);
        }
        else
            emit syncStateChanged(NotValidated);
    }
}

void AccountSyncHandler::releaseStoreLock(AbstractSetupContext *context)
{
    qDebug() << Q_FUNC_INFO;
    Q_D(AccountSyncHandler);

    if (context != 0) {
        disconnect(context, SIGNAL(stored()),
                   this, SLOT(onContextStored()));
        disconnect(context, SIGNAL(error(AccountsUI::ErrorCode, const QString &)),
                   this, SLOT(onContextError(AccountsUI::ErrorCode, const QString &)));
    }
    Q_ASSERT(d->storeLocks > 0);
    d->storeLocks--;

    if (d->storeLocks == 0) {
        qDebug() << "All contexts replied";


        /* All contexts have replied */
        if (!d->storeError) {
            emit syncStateChanged(Stored);
        }
        else
            emit syncStateChanged(NotStored);
    }
}

void AccountSyncHandler::validate(QList<AbstractSetupContext*> contexts)
{
    qDebug() << Q_FUNC_INFO;
    Q_D(AccountSyncHandler);

    d->validateLocks = contexts.count();

    /* clear the error state */
    d->validateError = false;

    /* Validate */

    foreach (AbstractSetupContext *context, contexts) {
        connect(context, SIGNAL(validated()),
                this, SLOT(onContextValidated()));
        connect(context, SIGNAL(error(AccountsUI::ErrorCode,
                                      const QString &)),
                this, SLOT(onContextValidateError(AccountsUI::ErrorCode,
                                                  const QString &)));
        context->validate();
    }
}

void AccountSyncHandler::onContextValidated()
{
    qDebug() << Q_FUNC_INFO;
    AbstractSetupContext *context;
    context = qobject_cast<AbstractSetupContext *>(QObject::sender());

    releaseValidateLock(context);
}

void AccountSyncHandler::onContextValidateError(AccountsUI::ErrorCode code,
                                                 const QString &message)
{
    qDebug() << Q_FUNC_INFO;
    Q_D(AccountSyncHandler);
    Q_UNUSED(code);

    d->validateError = true;

    AbstractServiceSetupContext *serviceContext
        = qobject_cast<AbstractServiceSetupContext *>(QObject::sender());
    if (serviceContext != 0) {
        const Accounts::Service *service = serviceContext->service();
        //% "Values for %1 wrong. %2"
        QString errMessage = qtTrId("qtn_acc_creds_validation_failed")
                             .arg(service->name()).arg(message);

        qDebug() << errMessage;

        showInfoBanner(errMessage);

        releaseValidateLock(serviceContext);

        return;
    }

    AbstractSetupContext *context = qobject_cast<AbstractSetupContext *>(QObject::sender());
    showInfoBanner(message);

    qDebug() << message;
    releaseValidateLock(context);
}

void AccountSyncHandler::store(QList<AbstractSetupContext*> contexts)
{
    qDebug() << Q_FUNC_INFO;
    Q_D(AccountSyncHandler);

    d->storeLocks = contexts.count();

    /* clear the error state */
    d->storeError = false;

    /* Store the service settings */
    foreach (AbstractSetupContext *context, contexts) {
        connect(context, SIGNAL(stored()),
                this, SLOT(onContextStored()));
        connect(context, SIGNAL(error(AccountsUI::ErrorCode,
                                      const QString &)),
                this, SLOT(onContextError(AccountsUI::ErrorCode,
                                          const QString &)));
        context->store();
    }
}

void AccountSyncHandler::onContextStored()
{
    qDebug() << Q_FUNC_INFO;

    AbstractSetupContext *context;
    context = qobject_cast<AbstractSetupContext *>(QObject::sender());

    releaseStoreLock(context);
}

void AccountSyncHandler::onContextError(AccountsUI::ErrorCode code,
                                         const QString &message)
{
    qDebug() << Q_FUNC_INFO;
    Q_D(AccountSyncHandler);
    Q_UNUSED(code);

    d->storeError = true;

    AbstractSetupContext *context;
    context = qobject_cast<AbstractSetupContext *>(QObject::sender());

    QString errMessage = qtTrId("qtn_acc_storing_err_undefined");

    qDebug() << message;

    showInfoBanner(errMessage);

    releaseStoreLock(context);
}

} //namespace
