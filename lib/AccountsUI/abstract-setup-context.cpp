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

#include "abstract-setup-context.h"

using namespace Accounts;

namespace AccountsUI {

class AbstractSetupContextPrivate
{
public:
    AbstractSetupContextPrivate()
            : account(0)
    {
    }

    Account *account;
};

AbstractSetupContext::AbstractSetupContext(Accounts::Account *account,
                                           QObject *parent) :
    QObject(parent),
    d_ptr(new AbstractSetupContextPrivate())
{
    Q_D(AbstractSetupContext);

    d->account = account;
}

AbstractSetupContext::~AbstractSetupContext()
{
    delete d_ptr;
}

Account *AbstractSetupContext::account()
{
    Q_D(AbstractSetupContext);

    return d->account;
}

void AbstractSetupContext::abort()
{
}

void AbstractSetupContext::validate()
{
    emit validated();
}

} // namespace
