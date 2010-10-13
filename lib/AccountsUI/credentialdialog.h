/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 * Contact: Lucian Horga <ext-lucian.horga@nokia.com>
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

#ifndef ACCOUNTSUI_CREDENTIALDIALOG_H
#define ACCOUNTSUI_CREDENTIALDIALOG_H

#include <AccountsUI/common.h>

//Qt
#include <QObject>
#include <QModelIndex>

//M
#include <MDialog>
#include <MInfoBanner>
#include <MMessageBox>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLabel>
#include <MTextEdit>

//SignOn
#include <identity.h>
#include <identityinfo.h>

using namespace SignOn;

class CredentialWidgetModel;

class ACCOUNTSUI_EXPORT CredentialDialog: public MDialog
{
    Q_OBJECT

public:
    CredentialDialog(const qint32 credentialsId);

    ~CredentialDialog();

private:
    void initDialog();

private Q_SLOTS:
    void deleteClicked();
    void saveClicked();
    void cancelClicked();

    void removedIdentity();
    void infoReady(const SignOn::IdentityInfo &info);
    void setDialogData(const SignOn::IdentityInfo &info);
    void credentialsStored(const quint32 credentialsId);
    void passwordChanged();

    void error(const SignOn::Error &err);
    void handleRejected();

    void clearPasswordField();

Q_SIGNALS:
    void safeToDeleteMe(CredentialDialog *ptr);

private:
    QModelIndex itemModelIndex;

    QObject *deleteButtonModel;
    QObject *cancelButtonModel;
    QObject *saveButtonModel;

    SignOn::Identity *identity;

    MWidget *centralWidget;
    MLayout *mainLayout;
    MGridLayoutPolicy *singlePolicy;
    MLabel *usernameLabel;
    MLabel *passwordLabel;
    MTextEdit *passwordTextEdit;

    bool passwordSetByUser;
    QString username;
};

#endif  // ACCOUNTSUI_CREDENTIALDIALOG_H

