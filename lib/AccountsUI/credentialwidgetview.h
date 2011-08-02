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

#ifndef ACCOUNTSUI_CREDENTIALWIDGETVIEW_H
#define ACCOUNTSUI_CREDENTIALWIDGETVIEW_H

//Qt
#include <QGraphicsSceneMouseEvent>
#include <QList>

//M
#include <MWidgetView>

//project
#include <AccountsUI/credentialwidget.h>
#include <AccountsUI/credentialwidgetmodel.h>
#include <AccountsUI/credentialwidgetstyle.h>
#include <AccountsUI/common.h>

//SignOn
#include <AuthService>
#include <Identity>

class CredentialWidgetViewPrivate;

class ACCOUNTSUI_EXPORT CredentialWidgetView: public MWidgetView
{
    Q_OBJECT
    M_VIEW(CredentialWidgetModel, CredentialWidgetStyle)

public:
    CredentialWidgetView(CredentialWidget *controller);
    ~CredentialWidgetView();

protected slots:
    // specific modifications in the model
    virtual void updateData(const QList <const char *> &modifications);

private Q_SLOTS:
    void refreshUsernameInModel();
    void refreshPasswordInModel();
    void refreshCaptchaTextInModel();
    void refreshCheckboxStateInModel(bool checked);

    void updateMainLayout();
    void recreateWidgets();

    void usernameTextEditGainedFocus();
    void launchKeychain();
    void infoReadySlot(const SignOn::IdentityInfo &info);
    void itemPickedSlot(quint32 credentialsId);
    void captchaImageClicked();
    void forgotPasswordClicked(QString link);
    void setEnabled(bool isWidgetEnabled);
    void passwordTextEditSetFocus();
    void closeVKB();
    void passwordTextEditGainedFocus();
    void onUsernameTextChanged();
    void onPasswordTextChanged();

private:
    void configureWithCaptcha(int &portraitRow, int &landscapeRow);
    void configureWithLogin(int &portraitRow, int &landscapeRow);
    void configureWithButtons(int &portraitRow, int &landscapeRow);
    void configureWithInfoLabel(int &portraitRow, int &landscapeRow);

protected:
    virtual void setupModel();
    virtual void applyStyle();

public:
    void setFocusOnUserNameField();
    void setFocusOnPasswordField();

signals:
    void enterCredentialsDone();
protected:
    CredentialWidgetViewPrivate *d_ptr;
    Q_DISABLE_COPY(CredentialWidgetView);
    Q_DECLARE_PRIVATE(CredentialWidgetView);
};

#endif // ACCOUNTSUI_CREDENTIALWIDGETVIEW_H
