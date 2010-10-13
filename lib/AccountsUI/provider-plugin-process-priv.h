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

#ifndef ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_PRIV_H
#define ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_PRIV_H

#include "provider-plugin-process.h"

#include <Accounts/Account>
#include <Accounts/Manager>
#include <MComponentCache>
#include <MApplication>
#include <MApplicationWindow>

namespace AccountsUI {

class ProviderPluginProcessPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProviderPluginProcess)

public:
    ProviderPluginProcessPrivate(AccountPluginInterface *plugin, int &argc, char **argv)
        : plugin(plugin)
        , m_context(0)
        , windowId(0)
        , serviceType(QString())
    {
        application = MComponentCache::mApplication(argc, argv);
        window = MComponentCache::mApplicationWindow();
    }

    ProviderPluginProcessPrivate(int &argc, char **argv)
        : m_context(0)
        , windowId(0)
    {
        application = MComponentCache::mApplication(argc, argv);
        window = MComponentCache::mApplicationWindow();
    }

    ~ProviderPluginProcessPrivate()
    {
        delete m_context;
    }

    void bindPageToAccountsUi(MApplicationPage *page);
    void printAccountId();
    AbstractAccountSetupContext *context() const;
    void serviceEnabled(Accounts::Service *service);
    void monitorServices();

public Q_SLOTS:
    void accountSaved();

private:
    mutable ProviderPluginProcess *q_ptr;
    Accounts::ServiceList enabledServices;
    AccountPluginInterface *plugin;
    MApplication *application;
    MApplicationWindow *window;
    mutable AbstractAccountSetupContext *m_context;
    Accounts::Manager *manager;
    int windowId;
    Accounts::Account *account;
    SetupType setupType;
    QString serviceType;
};

} // namespace
#endif // ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_PRIV_H
