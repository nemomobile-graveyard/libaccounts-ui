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

#include <QCoreApplication>
#include <QDebug>
#include <QLocalServer>
#include <QLocalSocket>
#include <QProcess>

using namespace Accounts;
using namespace AccountsUI;

ProviderPluginProxyWrapper::ProviderPluginProxyWrapper(QObject *parent):
    AccountSetup::ProviderPluginProxy(parent)
{
    QStringList dirs;

    dirs << QString::fromLatin1("/usr/libexec/AccountsUI") <<
        pluginDirectories();
    setPluginDirectories(dirs);

    /* Get MeegoTouch output level. Unfortunately libMeegoTouch does not
     * provide an API for this.
     */
    QStringList args = QCoreApplication::arguments();
    int index = args.indexOf(QLatin1String("-output-level"));
    if (index >= 0) {
        outputLevel = args[index + 1];
    }
}

QStringList ProviderPluginProxyWrapper::lastPageArguments(const LastPageActions
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

    return arguments;
}

void ProviderPluginProxyWrapper::setup(const LastPageActions *lastPageActions)
{
    QStringList parameters = lastPageArguments(lastPageActions);

    // Add MeegoTouch debugging option
    if (!outputLevel.isEmpty())
        parameters << QLatin1String("-output-level") << outputLevel;

    setAdditionalParameters(parameters);
    setParentWidget(MApplication::instance()->activeWindow());
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
        if (wrapper->exitData() == ProviderPluginProxy::EDIT_EXISTING_ACCOUNT)
            emit q->editExistingAccount(wrapper->createdAccountId());
        else
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

void ProviderPluginProxy::createAccount(Provider provider,
                                        const QString &serviceType)
{
    LastPageActions lastPageActions;
    return createAccount(provider, serviceType, lastPageActions);
}

void ProviderPluginProxy::createAccount(Provider provider,
                                        const QString &serviceType,
                                        const LastPageActions &lastPageActions)
{
    Q_D(ProviderPluginProxy);

    d->wrapper->setup(&lastPageActions);
    d->wrapper->createAccount(provider, serviceType);
}

void ProviderPluginProxy::editAccount(Account *account,
                                      const QString &serviceType)
{
    Q_D(ProviderPluginProxy);

    LastPageActions lastPageActions;
    d->wrapper->setup(&lastPageActions);
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

QString ProviderPluginProxy::providerName()
{
    Q_D(ProviderPluginProxy);
    return d->wrapper->providerName();
}

QVariant ProviderPluginProxy::exitData()
{
    Q_D(ProviderPluginProxy);
    return d->wrapper->exitData();
}

