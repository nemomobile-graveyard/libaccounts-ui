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

#ifndef ACCOUNTSUI_VALIDATION_DATA_H
#define ACCOUNTSUI_VALIDATION_DATA_H

// libAccountsUI
#include <AccountsUI/common.h>

// Qt
#include <QString>
#include <QVariantMap>

namespace AccountsUI {

/*!
 * @class ValidationData
 * @headerfile AccountsUI/validation-data.h \
 * AccountsUI/ValidationData
 * @brief Definition of the validation session
 *
 * The ValidationData class can be used to define the authentication session
 * that will take place when validating the account.
 */
class ACCOUNTSUI_EXPORT ValidationData
{
public:
    /*!
     * Constructor.
     */
    ValidationData(const QString &method,
                   const QString &mechanism,
                   const QVariantMap &sessionData):
        _method(method),
        _mechanism(mechanism),
        _sessionData(sessionData)
    {
    }

    virtual ~ValidationData() {}

    bool isNull() const { return _method.isNull(); }
    QString method() const { return _method; }
    QString mechanism() const { return _mechanism; }
    QVariantMap sessionData() const { return _sessionData; }

private:
    QString _method;
    QString _mechanism;
    QVariantMap _sessionData;
};

} // namespace

#endif // ACCOUNTSUI_VALIDATION_DATA_H
