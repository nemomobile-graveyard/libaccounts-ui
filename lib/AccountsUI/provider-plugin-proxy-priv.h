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

//libAccountsUI
#include "debug.h"
#include "last-page-actions.h"
#include "provider-plugin-proxy.h"

// AccountSetup
#include <AccountSetup/ProviderPluginProxy>

//Accounts
#include <Accounts/Manager>

//Qt
#include <QProcess>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QLocalServer>

//M
#include <MApplication>
#include <MWindow>

using namespace Accounts;
using namespace AccountsUI;

PWATCHER_INIT(pwatcher);

namespace AccountsUI {

class ProviderPluginProxyWrapper: public AccountSetup::ProviderPluginProxy
{
    Q_OBJECT
public:
    ProviderPluginProxyWrapper(QObject *parent);

    QStringList lastPageArguments(const LastPageActions *lastPageActions);
    void setup(const LastPageActions *lastPageActions);
    bool stopProcess();
private:
    QString outputLevel;
};

class ProviderPluginProxyPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProviderPluginProxy)

public:
    ProviderPluginProxyPrivate();
    ~ProviderPluginProxyPrivate() {};

private Q_SLOTS:
    void onPluginFinished();

private:
    mutable ProviderPluginProxy *q_ptr;
    ProviderPluginProxyWrapper *wrapper;
};

}; // namespace

#endif // ACCOUNTSUI_PROVIDER_PLUGIN_PROXY_PRIV_H

