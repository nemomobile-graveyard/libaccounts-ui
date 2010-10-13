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

#ifndef ACCOUNTSUI_NETWORKSESSIONMANAGER_H
#define ACCOUNTSUI_NETWORKSESSIONMANAGER_H

#include <AccountsUI/common.h>

#include <QObject>
#include <QMetaType>

namespace AccountsUI {

class NetworkSessionManagerPrivate;

class ACCOUNTSUI_EXPORT NetworkSessionManager : public QObject
{
    Q_OBJECT

public:

    enum ErrorType {
        Unknown = 0,
        Disconnected,
        Connection,
        Configuration
    };

    NetworkSessionManager(QObject *parent = 0);
    ~NetworkSessionManager();

    void startSession();
    void stopSession();

    bool isSessionReady();

Q_SIGNALS:
    void sessionReady();
    void error(const ErrorType);

private:
    NetworkSessionManagerPrivate *d_ptr;
    Q_DISABLE_COPY(NetworkSessionManager)
    Q_DECLARE_PRIVATE(NetworkSessionManager)
};

} //AccountsUI

Q_DECLARE_METATYPE(AccountsUI::NetworkSessionManager::ErrorType)

#endif //NETWORKSESSIONMANAGER_H


