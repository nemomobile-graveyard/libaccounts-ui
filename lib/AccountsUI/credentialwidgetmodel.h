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

#ifndef ACCOUNTSUI_CREDENTIALWIDGETMODEL_H
#define ACCOUNTSUI_CREDENTIALWIDGETMODEL_H

//libAccountsUI
#include <AccountsUI/common.h>

//M
#include <MWidgetModel>

class ACCOUNTSUI_EXPORT CredentialWidgetModel: public MWidgetModel
{
    Q_OBJECT
    M_MODEL(CredentialWidgetModel)

    /*!
        \property CredentialWidgetModel::DisplayName
        \brief .
    */
    M_MODEL_PROPERTY(QString, displayName, DisplayName, true, QString())

    /*!
        \property CredentialWidgetModel::Username
        \brief .
    */
    M_MODEL_PROPERTY(QString, username, Username, true, QString())

    /*!
        \property CredentialWidgetModel::UsernameEditable
        \brief .
    */
    M_MODEL_PROPERTY(bool, usernameEditable, UsernameEditable, true, true)

    /*!
        \property CredentialWidgetModel::ServiceIcon
        \brief .
    */
    M_MODEL_PROPERTY(QString, serviceIcon, ServiceIcon, true, QString())

    /*!
        \property CredentialWidgetModel::ConfirmPassword
        \brief .
    */
    M_MODEL_PROPERTY(bool, confirmPassword, ConfirmPassword, true, false)

    /*!
        \property CredentialWidgetModel::Password
        \brief .
    */
    M_MODEL_PROPERTY(QString, password, Password, true, QString())

    /*!
        \property CredentialWidgetModel::FocusOnPasswordField
        \brief .
    */
    M_MODEL_PROPERTY(bool, focusOnPasswordField, FocusOnPasswordField, true, false)

    /*!
        \property CredentialWidgetModel::ForgotPassword
        \brief .
    */
    M_MODEL_PROPERTY(QString, forgotPassword, ForgotPassword, true, QString())

    /*!
        \property CredentialWidgetModel::forgotPasswordVisible
        \brief .
    */
    M_MODEL_PROPERTY(bool, forgotPasswordVisible, ForgotPasswordVisible, true, false)

    /*!
        \property CredentialWidgetModel::ForgotPasswordUrl
        \brief .
    */
    M_MODEL_PROPERTY(QString, forgotPasswordUrl, ForgotPasswordUrl, true, QString())

    /*!
        \property CredentialWidgetModel::checkboxVisible
        \brief .
    */
    M_MODEL_PROPERTY(bool, checkboxVisible, CheckboxVisible, true, false)

    /*!
        \property CredentialWidgetModel::checkboxVisible
        \brief .
    */
    M_MODEL_PROPERTY(bool, checkboxPressed, CheckboxPressed, true, true)

    /*!
        \property CredentialWidgetModel::DialogsVisabilityConfig
        \brief .
    */
    M_MODEL_PROPERTY(int, dialogsVisabilityConfig, DialogsVisabilityConfig, true, 0x00)

    /*!
        \property CredentialWidgetModel::captcha
        \brief .
    */
    M_MODEL_PROPERTY(QImage, captcha, Captcha, true, QImage())

    /*!
        \property CredentialWidgetModel::captchaText
        \brief .
    */
    M_MODEL_PROPERTY(QString, captchaText, CaptchaText, true, QString())

    /*!
        \property CredentialWidgetModel::focusOnCaptchaTextField
        \brief .
    */
    M_MODEL_PROPERTY(bool, focusOnCaptchaTextField, FocusOnCaptchaTextField, true, false)

    /*!
        \property CredentialWidgetModel::signInButtonVisible
        \brief .
    */
    M_MODEL_PROPERTY(bool, signInButtonVisible, SignInButtonVisible, true, false)

    /*!
        \property CredentialWidgetModel::nextButtonVisible
        \brief .
    */
    M_MODEL_PROPERTY(bool, nextButtonVisible, NextButtonVisible, true, false)

    /*!
        \property CredentialWidgetModel::cancelButtonVisible
        \brief .
    */
    M_MODEL_PROPERTY(bool, cancelButtonVisible, CancelButtonVisible, true, false)

    /*!
        \property CredentialWidgetModel::enabled
        \brief .Property holdig whether the whole widget is enabled or not
    */
    M_MODEL_PROPERTY(bool, enabled, Enabled, true, true)

    /*!
        \property CredentialWidgetModel::forcedPortraitMode
        \brief .Property holding whether the whole widget is always shown in portrait layout.
                This is specially useful in the Signon-Ui credentials dialog.
    */
    M_MODEL_PROPERTY(bool, forcedPortraitMode, ForcedPortraitMode, true, false)

    /*!
        \property CredentialWidgetModel::informativeNoteText
        \brief .Property holding the text for any informative note the widget might display.
    */
    M_MODEL_PROPERTY(QString, informativeNoteText, InformativeNoteText, true, QString())

    /*!
        \property CredentialWidgetModel::Authentication Failed
        \brief .
    */
    M_MODEL_PROPERTY(bool, authenticationFailed, AuthenticationFailed, true, false)

    public:

        enum Configuration {
            NoDialogs = 0x00,
            LoginDialogVisible = 0x01,
            CaptchaDialogVisible = 0x02
        };

        void captchaClick() {
            emit captchaClicked();
        }

        void forgotPasswordClick(QString link) {
            emit forgotPasswordClicked(link);
        }

        bool loginDialogVisible() {
            return (dialogsVisabilityConfig() & CredentialWidgetModel::LoginDialogVisible);
        }

        bool captchaDialogVisible() {
            return (dialogsVisabilityConfig() & CredentialWidgetModel::CaptchaDialogVisible);
        }

    Q_SIGNALS:
        void captchaClicked();
        void signInClicked();
        void nextClicked();
        void cancelClicked();
        void forgotPasswordClicked(QString);
};


#endif // ACCOUNTSUI_CREDENTIALWIDGETMODEL_H

