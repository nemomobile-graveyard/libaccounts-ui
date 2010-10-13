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

#include "network-session-manager-priv.h"
#include "network-session-manager.h"

#include <QDebug>

namespace AccountsUI {

NetworkSessionManagerPrivate::NetworkSessionManagerPrivate(NetworkSessionManager *parent)
    : q_ptr(parent),
      networkSession(0),
#ifdef __ARMEL__
      isSessionReady(false)
#else
      isSessionReady(true)
#endif
{}

NetworkSessionManagerPrivate::~NetworkSessionManagerPrivate()
{
    if (networkSession)
        delete networkSession;
}

void NetworkSessionManagerPrivate::startSession()
{
    Q_Q(NetworkSessionManager);

#ifndef __ARMEL__
    emit q->sessionReady();
    return;
#endif

    if (networkSession && !isSessionReady) {
        networkSession->open();
    } else {

        QNetworkConfigurationManager manager;
        QNetworkConfiguration cfg = manager.defaultConfiguration();

        if (!cfg.isValid()) {
            QLatin1String msg("Network configuration manager -"
                          "invalid default configuration");
            qWarning() << msg;
            emit q->error(NetworkSessionManager::Configuration);
            return;
        }

        if (networkSession == 0) {
            networkSession = new QNetworkSession(cfg);

            connect(networkSession,
                    SIGNAL(error(QNetworkSession::SessionError)),
                    SLOT(sessionErrorHandler(QNetworkSession::SessionError)));
            connect(networkSession,
                    SIGNAL(stateChanged(QNetworkSession::State)),
                    SLOT(sessionStateHandler(QNetworkSession::State)));
            connect(networkSession, SIGNAL(opened()), SLOT(sessionOpened()));
        }

        // pops up connectivity if needed
        networkSession->open();
    }
}

void NetworkSessionManagerPrivate::stopSession()
{
#ifndef __ARMEL__
    return;
#endif

    if (networkSession != 0)
        networkSession->close();
}

void NetworkSessionManagerPrivate::sessionOpened()
{
#ifndef __ARMEL__
    return;
#endif
    Q_Q(NetworkSessionManager);
    if (networkSession->state() == QNetworkSession::Connected) {
        if (!isSessionReady) {
            isSessionReady = true;
            emit q->sessionReady();
            qDebug() << "NetworkSessionManagerPrivate::sessionOpened() SESSION ACTIVE";
        }
    }
}

void NetworkSessionManagerPrivate::sessionStateHandler(QNetworkSession::State state)
{
#ifndef __ARMEL__
    Q_UNUSED(state)
    return;
#endif

    Q_Q(NetworkSessionManager);

    switch (state) {
    case QNetworkSession::Invalid:
        qDebug() << Q_FUNC_INFO << "Invalid network state";
        isSessionReady = false;
        break;
    case QNetworkSession::NotAvailable:
        qDebug() << Q_FUNC_INFO << "NotAvailable network state";
        isSessionReady = false;
        q->error(NetworkSessionManager::Connection);
        break;
    case QNetworkSession::Connecting:
        qDebug() << Q_FUNC_INFO << "Connecting network state";
        isSessionReady = false;
        break;
    case QNetworkSession::Connected:
        qDebug() << Q_FUNC_INFO << "Connected network state";
        if (networkSession->isOpen()) {
            if (!isSessionReady) {
                isSessionReady = true;
                emit q->sessionReady();
            }
        }
        break;
    case QNetworkSession::Closing:
        qDebug() << Q_FUNC_INFO << "Closing network state";
        q->error(NetworkSessionManager::Disconnected);
        isSessionReady = false;
        break;
    case QNetworkSession::Disconnected:
        qDebug() << Q_FUNC_INFO << "Disconnected network state";
        q->error(NetworkSessionManager::Disconnected);
        isSessionReady = false;
        break;
    case QNetworkSession::Roaming:
        qDebug() << Q_FUNC_INFO << "Roaming network state";
        isSessionReady = false;
        break;
    default:
        qWarning() << Q_FUNC_INFO << "Unknown network state";
        isSessionReady = false;
    }
}

void NetworkSessionManagerPrivate::sessionErrorHandler(QNetworkSession::SessionError err)
{
#ifndef __ARMEL__
    Q_UNUSED(err)
    return;
#endif

    Q_Q(NetworkSessionManager);

    switch(err) {
    case QNetworkSession::UnknownSessionError:
        qWarning() << Q_FUNC_INFO << "Network session UnknownSessionError";
        emit q->error(NetworkSessionManager::Unknown);
        break;
    case QNetworkSession::SessionAbortedError:
        qWarning() << Q_FUNC_INFO << "Network session SessionAbortedError";
        emit q->error(NetworkSessionManager::Connection);
        break;
    case QNetworkSession::RoamingError:
        qWarning() << Q_FUNC_INFO << "Network session RoamingError";
        emit q->error(NetworkSessionManager::Connection);
        break;
    case QNetworkSession::OperationNotSupportedError:
        qWarning() << Q_FUNC_INFO << "Network session OperationNotSupportedError";
        emit q->error(NetworkSessionManager::Configuration);
        break;
    case QNetworkSession::InvalidConfigurationError:
        qWarning() << Q_FUNC_INFO << "Network session InvalidConfigurationError";
        emit q->error(NetworkSessionManager::Configuration);
        break;
    default:
        qWarning() << Q_FUNC_INFO << "Unhandled Network session error";
        emit q->error(NetworkSessionManager::Unknown);
    }
}

} //AccountsUI
