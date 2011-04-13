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

#ifndef ACCOUNTSUI_COMMON_H
#define ACCOUNTSUI_COMMON_H

//Qt
#include <QtCore/QtGlobal>
#include <QString>

//SignOn
#include <SignOn/Error>

//M
#ifndef ACCOUNTSUI_DISABLE_DEPRECATED
#include <MInfoBanner>
#endif

#define ACCOUNTSUI_EXPORT Q_DECL_EXPORT

namespace AccountsUI {

typedef SignOn::Error::ErrorType SignonErrType;

/*!
 * Helper function. Maps SignOn error types to logical translation ids.
 * @param err the SignOn error type.
 * @param providerName the provider name.
 * @returns corresponding translation id string.
 */
const QString trIdFromSignonError(const SignonErrType err, const QString& providerName = NULL);

/*!
 * @overload trIdFromSignonError(const SignonErrType err)
 */
const QString trIdFromSignonError(const int err);

#ifndef ACCOUNTSUI_DISABLE_DEPRECATED
/*!
 * Displays an information banner.
 * @param text Text to be displayed.
 * @param type The type of the info banner
 * @param disapperTimeout Amount of seconds after which the banner disappears.
 *
 * @deprecated This function is deprecated and will be removed in a future
 * version.
 */
ACCOUNTSUI_EXPORT
void showInfoBanner(const QString &text,
                    const MInfoBanner::BannerType type = MInfoBanner::Information,
                    const quint32 disapperTimeout = 3000);
#endif

}

#endif // ACCOUNTSUI_COMMON_H
