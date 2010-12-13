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

//libAccountsUI
#include <AccountsUI/provider-plugin-process.h>

//Accounts
#include <Accounts/account.h>
#include <Accounts/manager.h>

//M
#include <MComponentCache>
#include <MApplication>
#include <MApplicationWindow>
#include <MComponentData>
#include <MLocale>

#include <QLocalSocket>

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
        , returnToApp(false)
        , serverName(QString())
    {
        application = MComponentCache::mApplication(argc, argv);
        window = MComponentCache::mApplicationWindow();
    }

    ProviderPluginProcessPrivate(int &argc, char **argv)
        : m_context(0)
        , windowId(0)
    {
        account = 0;
        setupType = CreateNew;

        application = MComponentCache::mApplication(argc, argv);
        manager = new Accounts::Manager(this);

        /* parse command line options */
        bool type_set = false;
        for (int i = 0; i < argc; ++i)
        {
            Q_ASSERT(argv[i] != NULL);

            if ((strcmp(argv[i], "--create") == 0) && !type_set)
            {
                setupType = CreateNew;
                type_set = true;

                i++;
                if (i < argc) {
                    account = manager->createAccount(argv[i]);
                    serverName = argv[i];
                }
            }
            else if ((strcmp(argv[i], "--edit") == 0) && !type_set)
            {
                setupType = EditExisting;
                type_set = true;

                i++;
                if (i < argc)
                    account = manager->account(atoi(argv[i]));
            }
            else if (strcmp(argv[i], "--windowId") == 0)
            {
                i++;
                if (i < argc)
                    windowId = atoi(argv[i]);
                Q_ASSERT(windowId != 0);
            }
            else if (strcmp(argv[i], "--serviceType") == 0)
            {
                i++;
                if (i < argc)
                    serviceType = argv[i];
                Q_ASSERT(serviceType != 0);
            }
        }

        if (account != 0)
            monitorServices();

        if (windowId != 0) {
            MComponentData::ChainData chainData(windowId, QString());
            MComponentData::pushChainData(chainData);
        }

        window = new MApplicationWindow;

        MLocale locale;
        locale.installTrCatalog("accountssso");
        MLocale::setDefault(locale);
    }

    ~ProviderPluginProcessPrivate()
    {
        delete application;
        delete m_context;
    }

    void printAccountId();
    AbstractAccountSetupContext *context() const;
    void serviceEnabled(Accounts::Service *service);
    void monitorServices();

public Q_SLOTS:
    void accountSaved();
    void socketConnectionError(QLocalSocket::LocalSocketError errorStatus);

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
    bool returnToApp;
    QString serverName;
};

} // namespace
#endif // ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_PRIV_H
