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

#ifndef NETWORKSESSIONMANAGERPRIV_H
#define NETWORKSESSIONMANAGERPRIV_H

#include <QObject>

// TODO: replace with next lines when libbearer is done correctly
//#include <QNetworkSession>
//#include <QNetworkConfiguration>
//#include <QNetworkConfigurationManager>

#include <qnetworksession.h>
#include <qnetworkconfiguration.h>
#include <qnetworkconfigmanager.h>
// TODO: END

namespace AccountsUI {

class NetworkSessionManager;

class NetworkSessionManagerPrivate : public QObject
{
    Q_OBJECT

    NetworkSessionManagerPrivate(NetworkSessionManager *parent);
    ~NetworkSessionManagerPrivate();

    void startSession();
    void stopSession();

private Q_SLOTS:
    void sessionStateHandler(QNetworkSession::State state);
    void sessionErrorHandler(QNetworkSession::SessionError err);
    void sessionOpened();

private:
    NetworkSessionManager *q_ptr;
    Q_DECLARE_PUBLIC(NetworkSessionManager);
    Q_DISABLE_COPY(NetworkSessionManagerPrivate);

    QNetworkSession *networkSession;
    bool isSessionReady;
};

} //AccountsUI

#endif //NETWORKSESSIONMANAGERPRIV_H
