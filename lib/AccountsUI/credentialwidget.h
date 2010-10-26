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

#ifndef ACCOUNTSUI_CREDENTIALWIDGET_H
#define ACCOUNTSUI_CREDENTIALWIDGET_H

//libAccountsUI
#include <AccountsUI/common.h>
#include <AccountsUI/credentialwidget.h>
#include <AccountsUI/credentialwidgetmodel.h>

//M
#include <MWidgetController>
#include <MWidgetCreator>

/*!
 * \class CredentialWidget is the controller class for CredentialWidgetView and model classes
 * \brief Provides an interface to set username and description
 *        fields to the model and get password from the model and set the focus
 *        on the password field at display.
 *
 */
class ACCOUNTSUI_EXPORT CredentialWidget: public MWidgetController
{
    Q_OBJECT
    M_CONTROLLER(CredentialWidget)

public:
    CredentialWidget(CredentialWidgetModel *model = 0, QGraphicsItem *parent = 0);
    virtual ~CredentialWidget();

    QString username();
    QString password();
    QString captchaText();
    bool rememberPasswordChecked();
    bool enabled();

    void setUsername(const QString &aUserName);
    void setPassword(const QString &aPassword);
    void setCaptcha(const QImage &image);
    void setRememberPasswordChecked(bool checked);
    void setFocusOnPasswordField();
    void setFocusOnCaptchaField();
    void setEnabled(bool isWidgetEnabled);

protected:
    /*!
     * \brief by receiving this notification from the Window system
     *        controller can set the focus to the password field when the view is displayed
     * \param show event
     */
    virtual void showEvent(QShowEvent *event);

private:
    Q_DISABLE_COPY(CredentialWidget);
};

#endif // ACCOUNTSUI_CREDENTIALWIDGET_H
