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

#ifndef ACCOUNTSUI_GENERICACCOUNTSETUPCONTEXT_H
#define ACCOUNTSUI_GENERICACCOUNTSETUPCONTEXT_H

//project lib
#include <AccountsUI/abstract-account-setup-context.h>

//SignOn
#include <SignOn/Identity>
#include <SignOn/SessionData>
#include <SignOn/Error>

namespace AccountsUI {

class GenericAccountSetupContextPrivate;

class ACCOUNTSUI_EXPORT GenericAccountSetupContext :
    public AbstractAccountSetupContext
{
    Q_OBJECT
    typedef SignOn::Error Error;
    typedef SignOn::SessionData SessionData;

public:
    GenericAccountSetupContext(Accounts::Account *account,
                              AccountsUI::SetupType type,
                              QObject *parent = 0);
    virtual ~GenericAccountSetupContext();

    void store();
    MWidget *widget(QGraphicsItem *parent);

protected:
    SignOn::Identity *identity() const;
    QString userName() const;

public Q_SLOTS:
    virtual void validate();
    void startAuthSession();

    void authenticationDone(const SignOn::SessionData &data);
    void credentialsStored(const quint32 id);
    void credentialsReady(const quint32 id);

    void authSessionError(const SignOn::Error &err);
    void storeCredentialsError(const SignOn::Error &err);
    void networkSessionError();
    void storeIdentity();

private:
    void storeAccount();
    GenericAccountSetupContextPrivate *d_ptr;
    Q_DISABLE_COPY(GenericAccountSetupContext)
    Q_DECLARE_PRIVATE(GenericAccountSetupContext)
};

} // namespace

#endif // ACCOUNTSUI_GENERICACCOUNTSETUPCONTEXT_H
