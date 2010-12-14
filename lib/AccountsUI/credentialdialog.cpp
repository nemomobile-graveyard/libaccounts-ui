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

//libAccountsUI
#include "credentialdialog.h"

//M
#include <MBanner>

//Qt
#include <QDebug>

namespace AccountsUI {

CredentialDialog::CredentialDialog(const qint32 credentialsId, CredentialDialogType type)
        :MDialog()
{
    qDebug() << Q_FUNC_INFO;

    passwordSetByUser = false;
    initDialog();

    identity = SignOn::Identity::existingIdentity(credentialsId, this);

    //if there is no identity with that ID return
    if (!identity)
        return;

    //% "Save"
    saveButtonModel = (QObject*)addButton(qtTrId("qtn_comm_save"));
    saveButtonModel->setObjectName("wgSaveButton");
    connect(saveButtonModel, SIGNAL(clicked()), this, SLOT(saveClicked()));

    if (type == SaveCredsOrCancel) {

        //% "Cancel"
        cancelButtonModel = (QObject*)addButton(qtTrId("qtn_comm_cancel"));
        cancelButtonModel->setObjectName("wgCancelButton");
        connect(cancelButtonModel, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    } else {

        //% "Delete"
        deleteButtonModel = (QObject *)addButton(qtTrId("qtn_comm_delete"));
        deleteButtonModel->setObjectName("wgDeleteButton");
        connect(deleteButtonModel, SIGNAL(clicked()),
                this, SLOT(deleteClicked()));

        connect(identity, SIGNAL(removed()),
                this, SLOT(removedIdentity()));
    }

    connect(identity, SIGNAL(info(const SignOn::IdentityInfo&)),
            this, SLOT(setDialogData(const SignOn::IdentityInfo&)));
    connect(identity, SIGNAL(credentialsStored(const quint32)),
            this, SLOT(credentialsStored(const quint32)));
    connect(identity, SIGNAL(error(const SignOn::Error &)),
            this, SLOT(error(const SignOn::Error &)));

    connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));

    identity->queryInfo();
}

CredentialDialog::~CredentialDialog()
{
    qDebug() << Q_FUNC_INFO;
}

void CredentialDialog::initDialog()
{
    qDebug() << Q_FUNC_INFO;
    //% "Edit password"
    setTitle(qtTrId("qtn_acc_login_title_edit"));
    setStyleName("wgCredentialDialog");

    //layouts and policies init
    centralWidget = new MWidget();
    centralWidget->setObjectName("wgCentralWidget");

    mainLayout = new MLayout(centralWidget);

    singlePolicy = new MGridLayoutPolicy(mainLayout);

    mainLayout->setPolicy(singlePolicy);

    usernameLabel = new MLabel();
    usernameLabel->setStyleName("wgUsernameLabel");

    //% "Password"
    passwordLabel = new MLabel(qtTrId("qtn_acc_login_password"));
    passwordLabel->setStyleName("wgPasswordLabel");

    passwordTextEdit = new MTextEdit();
    passwordTextEdit->setObjectName("wgPasswordTextEdit");

    passwordTextEdit->setMaximumWidth(350);
    passwordTextEdit->setEchoMode(MTextEditModel::Password);

    singlePolicy->addItem(usernameLabel,0,0);
    singlePolicy->addItem(passwordLabel,1,0);
    singlePolicy->addItem(passwordTextEdit,2,0);

    setCentralWidget(centralWidget);
}

void CredentialDialog::passwordChanged()
{
    qDebug() << Q_FUNC_INFO;
    if (passwordTextEdit->text().isEmpty())
        return;

    passwordSetByUser = true;
    disconnect(passwordTextEdit, SIGNAL(textChanged()),
               this, SLOT(passwordChanged()));
}

void CredentialDialog::handleRejected()
{
    qDebug() << Q_FUNC_INFO;
    emit safeToDeleteMe(this);
}

void CredentialDialog::deleteClicked()
{
    qDebug() << Q_FUNC_INFO;
    //% "Delete %1 from your device?"
    MMessageBox messageBox(QString(qtTrId("qtn_acc_remove_account")).arg(username),
                           M::CancelButton | M::OkButton);
    messageBox.exec();
    if (messageBox.result() == MDialog::Accepted) {
        if (identity)
            identity->remove();
    }
}

void CredentialDialog::removedIdentity()
{
    qDebug() << Q_FUNC_INFO;

    emit credentialsDeleted(identity->id());

    if (identity) {
        delete identity;
        identity = NULL;
    }

    emit safeToDeleteMe(this);
}

void CredentialDialog::saveClicked()
{
    qDebug() << Q_FUNC_INFO;
    if (passwordSetByUser) {
        qDebug() << Q_FUNC_INFO << "Password set by user: updating";
        if (identity) {
            disconnect(identity, SIGNAL(info(const SignOn::IdentityInfo&)),
                       this, SLOT(setDialogData(const SignOn::IdentityInfo&)));
            connect(identity, SIGNAL(info(const SignOn::IdentityInfo&)),
                    this, SLOT(infoReady(const SignOn::IdentityInfo&)));
            identity->queryInfo();
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Password is not set by user: canceling";
        emit safeToDeleteMe(this);
    }

}

void CredentialDialog::cancelClicked()
{
    qDebug() << Q_FUNC_INFO;
    emit safeToDeleteMe(this);
}

void CredentialDialog::infoReady(const SignOn::IdentityInfo &info)
{
    qDebug() << Q_FUNC_INFO;
    SignOn::IdentityInfo updatedInfo(info);

    if (passwordSetByUser)
        updatedInfo.setSecret(passwordTextEdit->text(), true);

    if (identity)
        identity->storeCredentials(updatedInfo);
}

void CredentialDialog::setDialogData(const SignOn::IdentityInfo &info)
{
    qDebug() << Q_FUNC_INFO;
    username = info.userName();
    usernameLabel->setText(info.userName());
    passwordTextEdit->setText(QLatin1String("*******"));

    connect(passwordTextEdit, SIGNAL(textChanged()),
            this, SLOT(passwordChanged()));

    connect(passwordTextEdit, SIGNAL(gainedFocus(Qt::FocusReason)),
            this, SLOT(clearPasswordField()));
}

void CredentialDialog::clearPasswordField()
{
    passwordTextEdit->clear();
    disconnect(passwordTextEdit, SIGNAL(gainedFocus(Qt::FocusReason)),
               this, SLOT(clearPasswordField()));
}

void CredentialDialog::credentialsStored(const quint32 credentialsId)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(credentialsId)
    emit safeToDeleteMe(this);
}

void CredentialDialog::error(const SignOn::Error &err)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(err)

    MBanner *banner = new MBanner();

    //% "Database error. Please retry."
    banner->setStyleName("InformationBanner");
    banner->setTitle(qtTrId("qtn_database_error"));
    banner->appear(scene(), MSceneWindow::DestroyWhenDone);

    emit safeToDeleteMe(this);
}

} //namespace
