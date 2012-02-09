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

#include "common.h"

#include <MBanner>
#include <MNGFClient>

#include <sysinfo.h>

namespace AccountsUI {

ErrorTrHelper::ErrorTrHelper() : undefinedErr(-1) {}

void ErrorTrHelper::setErrorType(const int errType)
{
    this->errType = errType;
}

QString ErrorTrHelper::trId()
{
    errType = undefinedErr;
    return QString();
};

const QString trIdFromSignonError(const SignonErrType err,
                                  const QString& providerName)
{
    return trIdFromSignonError((SignonErrType)err, providerName, 0);
}

const QString trIdFromSignonError(const int err)
{
    return trIdFromSignonError((SignonErrType)err, QString(), 0);
}

const QString trIdFromSignonError(const int err, ErrorTrHelper *errTrHelper)
{
    return trIdFromSignonError((SignonErrType)err, QString(), errTrHelper);
}

const QString trIdFromSignonError(const SignOn::Error::ErrorType err,
                                  const QString& providerName,
                                  ErrorTrHelper *errTrHelper)
{
    //todo - enrich this
    switch (err) {
    case SignOn::Error::StoreFailed:
        //todo - dummy id, provide a real one
        //% "Unable to store credentials"
        return qtTrId("qtn_acc_creds_store_fail");
    case SignOn::Error::InvalidCredentials:
        //% "Invalid credentials"
        return qtTrId("qtn_acc_auth_failed_infobanner");
    case SignOn::Error::Network:
        //% "Service connection failed"
        return qtTrId("qtn_acc_conn_to_service_fail").arg(providerName);
    case SignOn::Error::NoConnection:
        //% "No network connection"
        return qtTrId("qtn_acc_conn_to_service_fail").arg(providerName);
    case SignOn::Error::NotAuthorized:
        //% "Wrong Credentials"
        return qtTrId("qtn_acc_auth_failed_infobanner");
    case SignOn::Error::IncorrectDate:
        //% "Date and time are wrong"
        return qtTrId("qtn_comm_share_incorrect_date");
    case SignOn::Error::SessionClosed:
        //% "Account creation failed."
        return qtTrId("qtn_acc_general_error_infobanner");
    default:
        if (errTrHelper != 0) {
            errTrHelper->setErrorType(static_cast<int>(err));
            QString trId = errTrHelper->trId();
            if (!trId.isEmpty()) return trId;
        }
        //todo - provide a generic error id
        return qtTrId("qtn_comm_general_error");
    }
}

void
ErrorMessageDisplayHelper::displayMessage(const QString &text,
                                          const ErrorContext errContext,
                                          const QString &providerName)
{
    Q_UNUSED(errContext)
    Q_UNUSED(providerName)
    switch (errContext) {
    case AccountValidationErr:
        /* fall through */
    case UserInputErr:
        /* fall through */
    case AccountSavingErr:
        /* fall through */
    default:
        showInfoBanner(text); break;
    }
}

void
ErrorMessageDisplayHelper::displayMessage(const int errCode,
                                          const ErrorContext errContext,
                                          const QString &providerName)
{
    const QString text = trIdFromSignonError(errCode);
    displayMessage(text, errContext, providerName);
}

void showInfoBanner(const QString &text,
                    const MInfoBanner::BannerType type,
                    const quint32 disapperTimeout,
                    const QString &feedback)
{
    Q_UNUSED(type);
    Q_UNUSED(disapperTimeout);

    MBanner *banner = new MBanner();
    banner->setStyleName("InformationBanner");
    banner->setTitle(text);
    banner->appear(MSceneWindow::DestroyWhenDone);
    if (!feedback.isEmpty()) {
        MNGFClient m_NgfClient;
        m_NgfClient.playEvent(feedback);
    }
}

const QString productNameTr()
{
    struct system_config *sc = 0;
    QByteArray name;

    if (sysinfo_init(&sc) == 0) {
        uint8_t *data = 0;
        unsigned long size = 0;


        if (sysinfo_get_value(sc, "/component/product-name",
                              &data, &size) == 0) {
            name = QByteArray((const char *)(data), size);
            free(data);
        }
        sysinfo_finish(sc);
    }
    QString productNameId;
    if (name == "N9" || name == "Nxy")
        productNameId = qtTrId("qtn_comm_product_n9");
    else
        productNameId = qtTrId("qtn_comm_product_nxx");

    return productNameId;
}

bool isChinaVariant()
{
    struct system_config *sc = 0;
    QByteArray name;

    if (sysinfo_init(&sc) == 0) {
        uint8_t *data = 0;
        unsigned long size = 0;

        if (sysinfo_get_value(sc, "/device/sw-release-ver",
                              &data, &size) == 0) {
            name = QByteArray((const char *)(data), size);
            free(data);
        }
        sysinfo_finish(sc);
    }
    return name.endsWith("003");
}

} //namespace
