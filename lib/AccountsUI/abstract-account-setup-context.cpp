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

#include "abstract-account-setup-context.h"

using namespace Accounts;

namespace AccountsUI {

class AbstractAccountSetupContextPrivate
{
public:
    AbstractAccountSetupContextPrivate()
        : serviceType(QString())
    {
    }

    SetupType setupType;
    QString serviceType;
};

AbstractAccountSetupContext::AbstractAccountSetupContext
    (Accounts::Account *account, SetupType type, QObject *parent):
        AbstractSetupContext(account, parent),
        d_ptr(new AbstractAccountSetupContextPrivate())
{
    Q_D(AbstractAccountSetupContext);
    d->setupType = type;
}

AbstractAccountSetupContext::~AbstractAccountSetupContext()
{
    delete d_ptr;
}

SetupType
AbstractAccountSetupContext::setupType()
{
    Q_D(AbstractAccountSetupContext);

    return d->setupType;
}

void
AbstractAccountSetupContext::setServiceType(const QString &serviceType)
{
    Q_D(AbstractAccountSetupContext);

    d->serviceType = serviceType;
}

const QString
AbstractAccountSetupContext::serviceType() const
{
    Q_D(const AbstractAccountSetupContext);

    return d->serviceType;
}

} // namespace
