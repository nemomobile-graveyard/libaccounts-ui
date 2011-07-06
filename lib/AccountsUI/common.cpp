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

#include <sysinfo.h>

namespace AccountsUI {

const QString trIdFromSignonError(const int err)
{
    //todo - if possible, add QFlag<> validation
    return trIdFromSignonError((SignonErrType)err);
}

const QString trIdFromSignonError(const SignOn::Error::ErrorType err, const QString& providerName)
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
        //% Date and time are wrong"
        return qtTrId("qtn_comm_share_incorrect_date");
    default:
        //todo - provide a generic error id
        return qtTrId("qtn_comm_general_error");
    }
}

void showInfoBanner(const QString &text,
                    const MInfoBanner::BannerType type,
                    const quint32 disapperTimeout)
{
    Q_UNUSED(type);
    Q_UNUSED(disapperTimeout);

    MBanner *banner = new MBanner();
    banner->setStyleName("InformationBanner");
    banner->setTitle(text);
    banner->appear(MSceneWindow::DestroyWhenDone);
}

const QString productNameTrId()
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
    if (name == "N9")
        productNameId = qtTrId("qtn_comm_product_n9");
    else
        productNameId = qtTrId("qtn_comm_product_nxx");

    return productNameId;
}

} //namespace
