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


ProviderPluginProxyPrivate::~ProviderPluginProxyPrivate()
{
    qDebug() << Q_FUNC_INFO;
    if (process) {
        stopProcess();
        delete process;
    }
}

void ProviderPluginProxyPrivate::startProcess(Provider *provider,
                                              AccountId accountId,
                                              const QString &serviceType)
{
    Q_Q(ProviderPluginProxy);

    PWATCHER_RESTART(pwatcher);
    const char pluginNamePattern[] = "%1plugin";

    /*
     * TODO: specify dir path by some variable
     * */
    QDir pluginDir(QString::fromLatin1("/usr/libexec/AccountsUI"));
    QDomElement root(provider->domDocument().documentElement());
    QString pluginTypeName(root.
                           firstChildElement(QString::fromLatin1("plugin")).
                           text());

    QString pluginFileName(QString::fromLatin1(pluginNamePattern)
                           .arg(pluginTypeName));
    QFileInfo pluginFileInfo(pluginDir, pluginFileName);

    QString providerId(root.attribute(QLatin1String("id")));

    //fallback use id as a plugin name
    if (!pluginFileInfo.exists()) {
        pluginFileName = QString::fromLatin1(pluginNamePattern).arg(providerId);
        pluginFileInfo.setFile(pluginDir, pluginFileName);

        if (!pluginFileInfo.exists()) {
            pluginFileName = QString::fromLatin1(pluginNamePattern).
                                      arg(QLatin1String("generic"));
            pluginFileInfo.setFile(pluginDir, pluginFileName);
        }
    }

    if (!pluginFileInfo.exists()) {
        emit q->failed();
        return;
    }

    Qt::HANDLE windowId = MApplication::instance()->activeWindow()->winId();
    pid_t pid = getpid();
    serverName = providerId + QString::number(pid);

    QString processName = pluginFileInfo.canonicalFilePath();
    QStringList arguments;
    arguments << QLatin1String("--serverName") << serverName;
    arguments << QLatin1String("--windowId") << QString::number(windowId);

    if (accountId != 0) {
        arguments << QLatin1String("--edit") << QString::number(accountId);
        newAccountCreation = false;
    } else {
        arguments << QLatin1String("--create") << providerId;
        newAccountCreation = true;
    }

    if (!serviceType.isEmpty())
        arguments << QLatin1String("--serviceType") << serviceType;

#ifndef QT_NO_DEBUG_OUTPUT
    arguments << QLatin1String("-output-level") << QLatin1String("debug");
#endif

    if (!process &&
        !(process = new QProcess())) {
        emit q->failed();
        return;
    }

    pluginName = pluginFileName;

    qDebug() << Q_FUNC_INFO << processName << arguments;

    connect(process, SIGNAL(readyReadStandardError()),
            this, SLOT(onReadStandardError()));
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(onError(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onFinished(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(started()), this, SLOT(setCommunicationChannel()));

    process->start(processName, arguments);
    PWATCHER_TRACE(pwatcher);
}

void ProviderPluginProxyPrivate::setCommunicationChannel()
{
    QLocalServer *server = new QLocalServer();
    QLocalServer::removeServer(serverName);
    if (!server->listen(serverName))
        qWarning() << "Server not up";
    else
        connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void ProviderPluginProxyPrivate::onNewConnection()
{
    QLocalServer *server = qobject_cast<QLocalServer*>(sender());
    QLocalSocket *socket = server->nextPendingConnection();
    if (!socket->waitForConnected()) {
        qWarning() << "Server Connection not established";
        return;
    }
    if (!socket->waitForReadyRead()) {
        qWarning() << "Server data not available for reading";
        return;
    }
    QByteArray ba = socket->readAll();
    QString data(ba);
    accountInfo = data;
    socket->close();
}


bool ProviderPluginProxyPrivate::stopProcess()
{
    if (process == 0)
        return false;
    process->disconnect();
    process->close();
    return process->waitForFinished();
}

void ProviderPluginProxyPrivate::onReadStandardError()
{
    qDebug() << QString::fromLatin1(process->readAllStandardError());
}

void ProviderPluginProxyPrivate::onError(QProcess::ProcessError err)
{
    Q_Q(ProviderPluginProxy);

    PWATCHER_TRACE(pwatcher);
    if (err == QProcess::FailedToStart) {
        pluginName.clear();

        emit q->failed();
        if (process) {
            process->deleteLater();
            process = NULL;
        }
    }

    qDebug() << "Error: " << err;
}

void ProviderPluginProxyPrivate::onFinished(int exitCode,
                                            QProcess::ExitStatus exitStatus)
{
    Q_Q(ProviderPluginProxy);
    Q_UNUSED(exitCode);
    PWATCHER_TRACE(pwatcher) << exitCode;
    int returnToApp = 0;
    int result = 0;

    if (exitStatus == QProcess::CrashExit) {
        emit q->failed();
        process->deleteLater();
        process = NULL;
        return;
    }

    if (newAccountCreation) {
        QString value;
        if (!accountInfo.isEmpty())
            value = accountInfo;
        else {
            char buffer[16];
            process->readLine(buffer, sizeof(buffer));
            value = QString::fromAscii(buffer);
        }
        if (!value.isEmpty()) {
            QStringList resultList = value.split(" ");
            result = resultList.at(0).toInt();
            if (resultList.count() > 1)
                returnToApp = resultList.at(1).toInt();
            /* TODO: do we need the returnToApp value?
             * - if not, remove it completely (and from ProviderPluginProcess
             * - if yes, consider exposing it as an additional method
             */
            Q_UNUSED(returnToApp);
        }

        qDebug() << "Plugin output: " << result;

        emit q->created(result);
    } else
        emit q->edited();

    pluginName.clear();

    if (process) {
        process->deleteLater();
        process = NULL;
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
    Q_D(ProviderPluginProxy);
    PWATCHER_TRACE(pwatcher);

    if (!provider) {
        qCritical() << " NULL pointer to provider";
        emit failed();
        return;
    }

    d->startProcess(provider, 0, serviceType);
}

void ProviderPluginProxy::editAccount(Account *account,
                                      const QString &serviceType)
{
    Q_D(ProviderPluginProxy);
    PWATCHER_TRACE(pwatcher);

    if (!account) {
        qCritical() << " NULL pointer to account";
        emit failed();
        return;
    }

    Manager *manager = account->manager();
    Provider *provider = manager->provider(account->providerName());
    d->startProcess(provider, account->id(), serviceType);
}

void ProviderPluginProxy::stopProcess()
{
    Q_D(ProviderPluginProxy);
    d->stopProcess();
}

bool ProviderPluginProxy::isPluginRunning()
{
    Q_D(ProviderPluginProxy);
    return d->process != NULL;
}

QString ProviderPluginProxy::pluginName()
{
    if (!isPluginRunning())
        return QString();

    Q_D(ProviderPluginProxy);
    return d->pluginName;
}

