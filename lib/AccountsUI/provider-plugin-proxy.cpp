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

#include "provider-plugin-proxy.h"
#include "provider-plugin-proxy-priv.h"

#include <QProcess>
#include <QDebug>
#include <QLocalServer>
#include <QLocalSocket>

using namespace Accounts;
using namespace AccountsUI;

void ProviderPluginProxyWrapper::setLastPageArguments(const LastPageActions
                                                      *lastPageActions)
{
    const LastPageActions::ServiceActionList actions =
        lastPageActions->serviceActions();

    QStringList arguments;
    foreach (LastPageActions::ServiceAction action, actions) {
        arguments << QLatin1String("--action")
            << action.title()
            << action.serviceName();
    }

    setAdditionalParameters(arguments);
}

bool ProviderPluginProxyWrapper::stopProcess()
{
    return killRunningPlugin();
}

ProviderPluginProxyPrivate::ProviderPluginProxyPrivate():
        wrapper(new ProviderPluginProxyWrapper(this))
{
    QObject::connect(wrapper, SIGNAL(finished()),
                     this, SLOT(onPluginFinished()));
}

void ProviderPluginProxyPrivate::onPluginFinished()
{
    Q_Q(ProviderPluginProxy);

    if (wrapper->error() != AccountSetup::ProviderPluginProxy::NoError) {
        emit q->failed();
    } else if (wrapper->setupType() == AccountSetup::CreateNew) {
        emit q->created(wrapper->createdAccountId());
    } else {
        emit q->edited();
    }
}

ProviderPluginProxy::ProviderPluginProxy(QObject *parent):
    QObject(parent),
    d_ptr(new ProviderPluginProxyPrivate())
{
    Q_D(ProviderPluginProxy);

    d->q_ptr = this;
}

ProviderPluginProxy::~ProviderPluginProxy()
{
    Q_D(ProviderPluginProxy);
    delete d;
}

void ProviderPluginProxy::createAccount(Provider *provider,
                                        const QString &serviceType)
{
    LastPageActions lastPageActions;
    return createAccount(provider, serviceType, lastPageActions);
}

void ProviderPluginProxy::createAccount(Provider *provider,
                                        const QString &serviceType,
                                        const LastPageActions &lastPageActions)
{
    Q_D(ProviderPluginProxy);

    d->wrapper->setLastPageArguments(&lastPageActions);
    d->wrapper->setParentWidget(MApplication::instance()->activeWindow());
    d->wrapper->createAccount(provider, serviceType);
}

void ProviderPluginProxy::editAccount(Account *account,
                                      const QString &serviceType)
{
    Q_D(ProviderPluginProxy);

    LastPageActions lastPageActions;
    d->wrapper->setLastPageArguments(&lastPageActions);
    d->wrapper->setParentWidget(MApplication::instance()->activeWindow());
    d->wrapper->editAccount(account, serviceType);
}

void ProviderPluginProxy::stopProcess()
{
    Q_D(ProviderPluginProxy);
    d->wrapper->stopProcess();
}

bool ProviderPluginProxy::isPluginRunning()
{
    Q_D(ProviderPluginProxy);
    return d->wrapper->isPluginRunning();
}

QString ProviderPluginProxy::pluginName()
{
    Q_D(ProviderPluginProxy);
    return d->wrapper->pluginName();
}

