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

#include "abstract-service-setup-context.h"

using namespace Accounts;

namespace AccountsUI {

class AbstractServiceSetupContextPrivate
{
public:
    AbstractServiceSetupContextPrivate():
        accountSetupContext(0)
    {
    }

    Service service;
    AbstractAccountSetupContext *accountSetupContext;
};

AbstractServiceSetupContext::AbstractServiceSetupContext
    (Accounts::Account *account,
     const Accounts::Service &service,
     QObject *parent)
    : AbstractSetupContext(account, parent),
    d_ptr(new AbstractServiceSetupContextPrivate())
{
    Q_D(AbstractServiceSetupContext);

    d->service = service;
}

AbstractServiceSetupContext::~AbstractServiceSetupContext()
{
    delete d_ptr;
}

const Service AbstractServiceSetupContext::service() const
{
    Q_D(const AbstractServiceSetupContext);

    return d->service;
}

void AbstractServiceSetupContext::setAccountSetupContext(
    AbstractAccountSetupContext *context)
{
    Q_D(AbstractServiceSetupContext);

    Q_ASSERT(d->accountSetupContext == 0);
    d->accountSetupContext = context;
}

AbstractAccountSetupContext *
AbstractServiceSetupContext::accountSetupContext() const
{
    Q_D(const AbstractServiceSetupContext);

    return d->accountSetupContext;
}

void AbstractServiceSetupContext::enable(bool enabled)
{
    Q_D(AbstractServiceSetupContext);

    Account *account = this->account();

    account->selectService(d->service);
    account->setEnabled(enabled);
}

bool AbstractServiceSetupContext::hasMandatorySettings() const
{
    return true;
}

} // namespace
