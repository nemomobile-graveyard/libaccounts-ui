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

#ifndef ACCOUNTSUI_ACCOUNT_SETUP_FINISHED_PAGE_PRIV_H
#define ACCOUNTSUI_ACCOUNT_SETUP_FINISHED_PAGE_PRIV_H

#include "account-setup-finished-page.h"
#include "common.h"

namespace AccountsUI {

class AccountSetupFinishedPagePrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(AccountSetupFinishedPage)

public:
    AccountSetupFinishedPagePrivate():
        QObject(0),
        q_ptr(0),
        account(0)
    {}

    ~AccountSetupFinishedPagePrivate() {}

private:
    mutable AccountSetupFinishedPage *q_ptr;
    Accounts::Account *account;
    QString serviceType;
};

} //namespace

#endif
