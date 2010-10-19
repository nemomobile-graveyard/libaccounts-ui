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

#ifndef ACCOUNTSUI_PROVIDER_PLUGIN_PROXY_PRIV_H
#define ACCOUNTSUI_PROVIDER_PLUGIN_PROXY_PRIV_H

#include "provider-plugin-proxy.h"
#include "debug.h"

#include <Accounts/Manager>

#include <QProcess>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#include <MApplication>
#include <MWindow>

using namespace Accounts;
using namespace AccountsUI;

PWATCHER_INIT(pwatcher);

namespace AccountsUI {

class ProviderPluginProxyPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProviderPluginProxy)

public:
    ProviderPluginProxyPrivate():
        pluginName(),
        process(0)
    {}
    ~ProviderPluginProxyPrivate();

    void startProcess(Provider *provider, AccountId accountId,
                      const QString &serviceType);
    bool stopProcess();

private Q_SLOTS:
    void onReadStandardError();
    void onError(QProcess::ProcessError);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    mutable ProviderPluginProxy *q_ptr;
    bool newAccountCreation;
    QString pluginName;
    QProcess *process;
};

}; // namespace

#endif // ACCOUNTSUI_PROVIDER_PLUGIN_PROXY_PRIV_H

