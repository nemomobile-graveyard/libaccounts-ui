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

#include "network-session-manager.h"
#include "network-session-manager-priv.h"

namespace AccountsUI {

NetworkSessionManager::NetworkSessionManager(QObject *parent)
    : QObject(parent),
      d_ptr(new NetworkSessionManagerPrivate(this))
{}

NetworkSessionManager::~NetworkSessionManager()
{
    if (d_ptr != 0)
        delete d_ptr;
}

bool NetworkSessionManager::isSessionReady()
{
    Q_D(NetworkSessionManager);
    return d->isSessionReady;
}

void NetworkSessionManager::startSession()
{
    Q_D(NetworkSessionManager);
    d->startSession();
}

void NetworkSessionManager::stopSession()
{
    Q_D(NetworkSessionManager);
    d->stopSession();
}

} //namespace
