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
#include <AccountsUI/last-page-actions.h>
#include <AccountsUI/provider-plugin-process.h>

// AccountSetup
#include <AccountSetup/ProviderPluginProcess>
#include "plugin-service.h"

//Accounts
#include <Accounts/account.h>
#include <Accounts/manager.h>

//M
#include <MComponentCache>
#include <MApplication>
#include <MApplicationWindow>
#include <MComponentData>
#include <MLocale>
#include <MApplicationService>

//Qt
#include <QLocalSocket>

namespace AccountsUI {

class ProviderPluginProcessPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProviderPluginProcess)

public:
    ProviderPluginProcessPrivate(AccountPluginInterface *plugin, int &argc, char **argv):
        wrapped(0),
        plugin(plugin),
        m_context(0),
        returnToApp(false)
    {
        service = new PluginService();
        QStringList argList = QString(*argv).split("/");
        /* set the serviceName as per the name of the plugin */
        service->setServiceName(QString("com.nokia.%1").
                                arg(argList.at(argList.count()-1)).toLatin1());
        application = MComponentCache::mApplication(argc, argv, QLatin1String(""), service);
        window = MComponentCache::mApplicationWindow();
        window->setStyleName("AccountsUiWindow");

#ifdef __ARMEL__
        window->setPortraitOrientation();
        window->setOrientationLocked(true);
#endif
    }

    ProviderPluginProcessPrivate(int &argc, char **argv):
        wrapped(0),
        m_context(0)
    {
        service = new PluginService();
        application = MComponentCache::mApplication(argc, argv, QString(), service);

        wrapped = new AccountSetup::ProviderPluginProcess(this);
        account = wrapped->account();

        service->setProviderName(account->providerName());
        service->registerService();
        /* parse command line options */
        for (int i = 0; i < argc; ++i)
        {
            Q_ASSERT(argv[i] != NULL);

            if (strcmp(argv[i], "--action") == 0)
            {
                Q_ASSERT (i + 2 < argc);
                lastPageActions.addServiceAction(QString::fromUtf8(argv[i + 1]),
                                                 QLatin1String(argv[i + 2]));
                i += 2;
            }
        }

        if (account != 0)
            monitorServices();

        WId windowId = wrapped->parentWindowId();
        if (windowId != 0) {
            MComponentData::ChainData chainData(windowId, QString());
            MComponentData::pushChainData(chainData);
        }

        window = new MApplicationWindow;
        window->setStyleName("AccountsUiWindow");

#ifdef __ARMEL__
        window->setPortraitOrientation();
        window->setOrientationLocked(true);
#endif

        MLocale locale;
        locale.installTrCatalog("accountssso");
        MLocale::setDefault(locale);
    }

    ~ProviderPluginProcessPrivate()
    {
        delete m_context;
        delete window;
        delete application;
    }

    AbstractAccountSetupContext *context() const;
    void serviceEnabled(Accounts::Service *service);
    void monitorServices();

public Q_SLOTS:
    void accountSaved();

private:
    mutable ProviderPluginProcess *q_ptr;
    AccountSetup::ProviderPluginProcess *wrapped;
    Accounts::ServiceList enabledServices;
    AccountPluginInterface *plugin;
    MApplication *application;
    MApplicationWindow *window;
    mutable AbstractAccountSetupContext *m_context;
    Accounts::Account *account;
    bool returnToApp;
    LastPageActions lastPageActions;
    PluginService *service;
};

} // namespace
#endif // ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_PRIV_H
