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

#include "provider-plugin-process.h"
#include "add-account-page.h"
#include "accountsettingspage.h"
#include "generic-account-setup-context.h"
#include "provider-plugin-process-priv.h"

#include <Accounts/Account>
#include <Accounts/Manager>
#include <MComponentCache>
#include <MApplication>
#include <MApplicationIfProxy>
#include <MApplicationWindow>

#include <QDebug>

namespace AccountsUI {

static ProviderPluginProcess *plugin_instance = 0;

void ProviderPluginProcessPrivate::printAccountId()
{
    Accounts::Account *account = context()->account();

    QFile output;
    output.open(STDOUT_FILENO, QIODevice::WriteOnly);
    QByteArray ba = QString("%1 %2").arg(account->id()).arg(QString::number(returnToApp)).toAscii();
    output.write(ba.constData());
    output.close();
}

AbstractAccountSetupContext *ProviderPluginProcessPrivate::context() const
{
    if (!m_context) {
        m_context = q_ptr->accountSetupContext(account, setupType, q_ptr);
        m_context->setServiceType(serviceType);
    }
    return m_context;
}

void ProviderPluginProcessPrivate::serviceEnabled(Accounts::Service *service)
{
    qDebug() << Q_FUNC_INFO << service->name();

    QDomElement root = service->domDocument().documentElement();
    QDomElement handler = root.firstChildElement("handler");
    if (!handler.isNull()) {
        QString type = handler.attribute("type");
        if (type == "command") {
            /* Syntax for the service file:
             *
             *   <handler type="command">/usr/bin/appname [args]...</handler>
             */
            QString command = handler.text();
            qDebug() << "Executing" << command;

            /* The account plugin process is going to die very soon; the
             * handler must be started as a detached process, for it to
             * continue to live. */
            bool ok = QProcess::startDetached(command);
            if (!ok)
                qWarning() << "Could not execute:" << command;
        }
        /* support more types (e.g., D-Bus services) here */
    }
}

void ProviderPluginProcessPrivate::accountSaved()
{
    qDebug() << Q_FUNC_INFO;

    account->selectService();
    if (account->enabled()) {
        /* Go through the enabled services and run the activation command, if
         * present */
        foreach (Accounts::Service *service, account->services()) {
            account->selectService(service);
            if (account->enabled() && !enabledServices.contains(service))
                serviceEnabled(service);
        }
    }
}

void ProviderPluginProcessPrivate::monitorServices()
{
    connect(account, SIGNAL(synced()), this, SLOT(accountSaved()));

    /* If we are editing an account, get the list of services initially
     * enabled, to avoid starting up their handlers for no reason */
    account->selectService();
    if (setupType == EditExisting && account->enabled()) {
        foreach (Accounts::Service *service, account->services()) {
            account->selectService(service);
            if (account->enabled())
                enabledServices.append(service);
        }
    }
}

ProviderPluginProcess::ProviderPluginProcess(AccountPluginInterface *plugin,
                                             int &argc, char **argv)
    : d_ptr(new ProviderPluginProcessPrivate(plugin, argc, argv))
{
    Q_D(ProviderPluginProcess);
    init(argc, argv);
    d->m_context = plugin->accountSetupContext(d->account, d->setupType, this);
    d->m_context->setServiceType(d->serviceType);
}

ProviderPluginProcess::ProviderPluginProcess(int &argc, char **argv)
    : d_ptr(new ProviderPluginProcessPrivate(argc, argv))
{
    init(argc, argv);
}

void ProviderPluginProcess::init(int &argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    Q_D(ProviderPluginProcess);
    d->q_ptr = this;
    if (plugin_instance != 0)
        qWarning() << "ProviderPluginProcess already instantiated";
    plugin_instance = this;
}

ProviderPluginProcess::~ProviderPluginProcess()
{
    Q_D(ProviderPluginProcess);

    delete d;
}

ProviderPluginProcess *ProviderPluginProcess::instance()
{
    return plugin_instance;
}

MApplicationPage * ProviderPluginProcess::mainPage()
{
    Q_D(ProviderPluginProcess);
    AbstractAccountSetupContext *context = d->context();

    if (context->setupType() == CreateNew)
        return new AddAccountPage(context);

    if (context->setupType() == EditExisting)
        return new AccountSettingsPage(context);

    /* we should never come to this point */
    Q_ASSERT(false);
    return 0;
}

int ProviderPluginProcess::exec()
{
    Q_D(ProviderPluginProcess);

    /* if we the account is invalid (either because it does not exists or
     * couldn't be loaded because of some DB error), return immediately */
    if (d->account == 0) {
        qWarning() << Q_FUNC_INFO << "account() is NULL";
        return 1;
    }

    d->window->show();
    MApplicationPage *page = mainPage();

    if (page == 0) {
        qWarning() << Q_FUNC_INFO << "The mainPage() returned 0";
        return 1;
    }

    page->appear(d->window);

    return d->application->exec();
}

void ProviderPluginProcess::quit()
{
    Q_D(ProviderPluginProcess);

    if (d->windowId != 0)
    {
        MApplicationIfProxy mApplicationIfProxy("com.nokia.accounts-ui",
                this);

        if (mApplicationIfProxy.connection().isConnected()) {
            mApplicationIfProxy.launch();
        }
    }

    d->printAccountId();

    QCoreApplication::exit(0);
}

AbstractAccountSetupContext *ProviderPluginProcess::setupContext() const
{
    Q_D(const ProviderPluginProcess);
    return d->context();
}

AbstractAccountSetupContext *ProviderPluginProcess::accountSetupContext(
            Accounts::Account *account,
            SetupType type,
            QObject *parent)
{
    return new GenericAccountSetupContext(account, type);
}

void ProviderPluginProcess::setReturnToApp(bool returnToApp)
{
    Q_D(ProviderPluginProcess);
    d->returnToApp = returnToApp;
}
} // namespace
