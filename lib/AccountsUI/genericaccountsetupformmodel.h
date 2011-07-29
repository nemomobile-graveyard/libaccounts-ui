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

#ifndef ACCOUNTSUI_GENERICACCOUNTSETUPFORMMODEL_H
#define ACCOUNTSUI_GENERICACCOUNTSETUPFORMMODEL_H

#include <AccountsUI/common.h>

#include <MWidgetModel>

class ACCOUNTSUI_EXPORT GenericAccountSetupFormModel : public MWidgetModel
{
    Q_OBJECT
    M_MODEL(GenericAccountSetupFormModel)

    M_MODEL_PROPERTY(QString, providerAccountDocumentAsString, ProviderAccountDocumentAsString, true, QString())
    M_MODEL_PROPERTY(QString, username, Username, true, QString())
    M_MODEL_PROPERTY(QString, usernameDisplayString, UsernameDisplayString, true, QString())
    M_MODEL_PROPERTY(QString, password, Password, true, QString())
    M_MODEL_PROPERTY(QString, providerName, ProviderName, true, QString())
    M_MODEL_PROPERTY(bool, rememberMe, RememberMe, true, true)
    M_MODEL_PROPERTY(bool, credentialWidgetEnabled, CredentialWidgetEnabled, true, true)
};

#endif // ACCOUNTSUI_GENERICACCOUNTSETUPFORMMODEL_H
