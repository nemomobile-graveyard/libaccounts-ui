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

#include <MImageWidget>

#include "generic-service-plugin.h"
#include "generic-service-setup-context.h"

using namespace Accounts;
using namespace AccountsUI;

MWidget *GenericServicePlugin::iconWidget(const QDomDocument &domDocument,
                                            QGraphicsItem *parent)
{
    QDomElement root =domDocument.documentElement();
    QDomElement iconElement = root.firstChildElement("icon");
    return new MImageWidget(iconElement.text(), parent);
}

AccountsUI::AbstractServiceSetupContext *GenericServicePlugin::serviceSetupContext(
        Accounts::Account *account,
        const Accounts::Service &service,
        QObject *parent)
{
    return new GenericServiceSetupContext(account, service, parent);
}
