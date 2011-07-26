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
#include "account-settings-page.h"
#include "generic-account-setup-context.h"
#include "provider-plugin-process-priv.h"
#include "accountsmanagersingleton.h"

#include <Accounts/Account>
#include <Accounts/Manager>

#include <MComponentCache>
#include <MApplication>
#include <MApplicationWindow>

#include <QDebug>
#include <QFile>
#include <QLocalSocket>
#include <QProcess>
#include <QTimer>
namespace AccountsUI {

static ProviderPluginProcess *plugin_instance = 0;

AbstractAccountSetupContext *ProviderPluginProcessPrivate::context() const
{
    if (!m_context) {
        SetupType setupType;
        switch (wrapped->setupType()) {
        case AccountSetup::CreateNew:
            setupType = CreateNew;
            break;
        case AccountSetup::EditExisting:
            setupType = EditExisting;
            break;
        default:
            qWarning() << "Setup type not recognized:" << wrapped->setupType();
            return 0;
        }

        m_context = q_ptr->accountSetupContext(account, setupType, q_ptr);
        m_context->setServiceType(wrapped->serviceType());
    }
    return m_context;
}

void ProviderPluginProcessPrivate::executeCommandFromXML(const QDomDocument
                                                         &document)
{
    QDomElement root = document.documentElement();
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

void ProviderPluginProcessPrivate::serviceEnabled(Accounts::Service *service)
{
    qDebug() << Q_FUNC_INFO << service->name();

    executeCommandFromXML(service->domDocument());

    // Commands might be specified in service-type files too
    Accounts::Manager *manager = account->manager();
    Accounts::ServiceType *serviceType =
        manager->serviceType(service->serviceType());
    if (serviceType != 0)
        executeCommandFromXML(serviceType->domDocument());
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
    if (wrapped->setupType() == AccountSetup::EditExisting &&
        account->enabled()) {
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
    init(argc, argv);
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

    if (!d->validProvider) {
        qCritical()<< Q_FUNC_INFO << "Not a Valid Provider";
        return 1;
    }

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

    int result = d->application->exec();
    return result;
}

void ProviderPluginProcess::quit()
{
    Q_D(ProviderPluginProcess);
    QTimer::singleShot(500, d->wrapped, SLOT(quit()));
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
    return new GenericAccountSetupContext(account, type, parent);
}

void ProviderPluginProcess::setReturnToApp(bool returnToApp)
{
    Q_D(ProviderPluginProcess);
    d->returnToApp = returnToApp;
}

void ProviderPluginProcess::setReturnToAccountsList(bool value)
{
    Q_D(ProviderPluginProcess);
    d->wrapped->setReturnToAccountsList(value);
}

void ProviderPluginProcess::setExitData(const QVariant &data)
{
    Q_D(ProviderPluginProcess);
    d->wrapped->setExitData(data);
}

const LastPageActions &ProviderPluginProcess::lastPageActions() const
{
    Q_D(const ProviderPluginProcess);
    return d->lastPageActions;
}

bool ProviderPluginProcess::isSetupComplete()
{
    Q_D(const ProviderPluginProcess);
    return d->accountSetupCompleted;
}

void ProviderPluginProcess::setSetupComplete(bool value)
{
    Q_D(ProviderPluginProcess);
    d->accountSetupCompleted = value;
}

QString ProviderPluginProcess::translatedProviderName() const
{
    Q_D(const ProviderPluginProcess);

    if (!(d->translatedProviderName .isEmpty()))
        return d->translatedProviderName;

    QString providerName(d->account->providerName());
    QString providerIconId;
    Accounts::Provider *provider =
            AccountsManager::instance()->provider(providerName);

    if (provider) {
        providerIconId = provider->iconName();
        QString catalog = provider->trCatalog();
        MLocale locale;
        if (!catalog.isEmpty() && !locale.isInstalledTrCatalog(catalog)) {
            locale.installTrCatalog(catalog);
            MLocale::setDefault(locale);
        }
    }

    d->translatedProviderName = qtTrId(provider->displayName().toLatin1());
    if (!(d->translatedProviderName .isEmpty()))
        return d->translatedProviderName;
    else if (!providerName.isEmpty())
        return providerName;
    else
        return QString();
}

void ProviderPluginProcess::setEditExistingAccount(Accounts::AccountId accountId)
{
    Q_D(ProviderPluginProcess);
    d->wrapped->setEditExistingAccount(accountId);
}

} // namespace
