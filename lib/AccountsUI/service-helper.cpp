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

#include "service-helper.h"
#include "abstract-account-setup-context.h"

//libAccountsUI
#include <AccountsUI/ServicePluginInterface>
#include <AccountsUI/GenericServiceSetupContext>

//Meegotouch
#include <MImageWidget>
#include <MLocale>

//Qt
#include <QDir>
#include <QPluginLoader>
#include <QDebug>

namespace AccountsUI {

class ServiceHelperPrivate
{
public:
    ServiceHelperPrivate(Accounts::Service p) :
        service(p), plugin(0)
    {
        domDocument = service.domDocument();
    }

    void loadPlugin();

    QDomDocument domDocument;
    Accounts::Service &service;
    ServicePluginInterface *plugin;
};

void ServiceHelperPrivate::loadPlugin()
{
    QString libPath = getenv("ACCOUNTS_UI_PLUGIN_DIR");
    if (!libPath.isEmpty()) {
        libPath = QDir::cleanPath(libPath) + QDir::separator();
    } else {
        libPath = "/usr/lib/AccountsUI/";
    }
    QString libName("lib%1plugin.so");

    //use the xml element plugin to get the plugin name
    QDomElement root = domDocument.documentElement();
    QDomElement pluginElement = root.firstChildElement("plugin");

    QString path;
    path = libPath + libName.arg(pluginElement.text());
    //fallback use id as a plugin name
    if (!QFile::exists(path)) {
        QString serviceId = root.attribute("id");
        path = libPath + libName.arg(serviceId);
    }

    QObject *plugin = 0;

    qDebug() << "Loading plugin " << path;
    QPluginLoader pluginLoader(path);
    if (QFile::exists(path)) {
        plugin = pluginLoader.instance();
    }

    this->plugin = qobject_cast<ServicePluginInterface *>(plugin);
}

#define RETURN_IF_PLUGIN_FAILED(retval) \
    if (!d->plugin) d->loadPlugin(); \
    if (!d->plugin) return retval

ServiceHelper::ServiceHelper(Accounts::Service service, QObject *parent)
    : QObject(parent),
    d_ptr(new ServiceHelperPrivate(service))
{
    Q_D(ServiceHelper);
    QString catalog = d->service.trCatalog();
    if (!catalog.isEmpty()) {
        MLocale locale;
        locale.installTrCatalog(catalog);
        MLocale::setDefault(locale);
    }
}

ServiceHelper::~ServiceHelper()
{
    Q_D(ServiceHelper);

    delete d;
}

Accounts::Service &ServiceHelper::service() const
{
    Q_D(const ServiceHelper);

    return d->service;
}

const QDomDocument ServiceHelper::domDocument() const
{
    Q_D(const ServiceHelper);

    return d->service.domDocument();
}

MWidget *ServiceHelper::iconWidget(QGraphicsItem *parent)
{
    Q_D(ServiceHelper);

    QDomElement root = d->domDocument.documentElement();
    QDomElement iconElement = root.firstChildElement("icon");
    if (iconElement.hasAttribute("dynamic") &&
        iconElement.attribute("dynamic") == "true")
    {
        /* we must get the value from the plugin */
        RETURN_IF_PLUGIN_FAILED(0);

        return d->plugin->iconWidget(d->domDocument, parent);
    }

    /* else, create the static icon widget here */
    return new MImageWidget(iconElement.text(), parent);
}


QString ServiceHelper::prettyName()
{
    Q_D(ServiceHelper);

    QString name;

    QDomElement root = d->domDocument.documentElement();
    QDomElement setupNameElement = root.firstChildElement("setupName");
    if (!setupNameElement.isNull())
        name = setupNameElement.text();
    else
        name = d->service.displayName();
    return qtTrId(name.toLatin1());
}

QString ServiceHelper::shortDescription()
{
    Q_D(ServiceHelper);
    QDomElement root = d->domDocument.documentElement();
    QDomElement descriptionElement = root.firstChildElement("short-description");
    if (!descriptionElement.isNull())
        return qtTrId(descriptionElement.text().toLatin1());
    else
        return QString();
}

QString ServiceHelper::description()
{
    Q_D(ServiceHelper);
    QDomElement root = d->domDocument.documentElement();
    QDomElement descriptionElement = root.firstChildElement("description");
    if (!descriptionElement.isNull())
        return qtTrId(descriptionElement.text().toLatin1());
    else
        return QString();
}

AbstractServiceSetupContext *
ServiceHelper::serviceSetupContext(AbstractAccountSetupContext *context,
                                   QObject *parent)
{
    Q_D(ServiceHelper);
    if (!d->plugin)
        d->loadPlugin();

    AbstractServiceSetupContext *serviceContext;
    if (d->plugin)
        serviceContext = d->plugin->serviceSetupContext(context->account(),
                                                        d->service, parent);
    else
        serviceContext = new GenericServiceSetupContext(context->account(),
                                                        d->service, parent);

    if (serviceContext)
        serviceContext->setAccountSetupContext(context);
    return serviceContext;
}

AbstractServiceSetupContext *
ServiceHelper::serviceSetupContext(Accounts::Account *account,
                                   QObject *parent)
{
    Q_D(ServiceHelper);
    if (!d->plugin)
        d->loadPlugin();
    if (d->plugin)
        return d->plugin->serviceSetupContext(account, d->service, parent);
    return new GenericServiceSetupContext(account, d->service, parent);
}

QString ServiceHelper::providerName()
{
    Q_D(ServiceHelper);
    return d->service.provider();
}

QString ServiceHelper::serviceType()
{
    Q_D(ServiceHelper);
    return d->service.serviceType();
}

QString ServiceHelper::serviceName()
{
    Q_D(ServiceHelper);
    return d->service.name();
}

} // namespace
