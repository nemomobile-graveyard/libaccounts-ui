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

//project
#include "credentialwidgetexample.h"

//M
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MLabel>
#include <MTextEdit>
#include <MWidgetAction>
#include <MGridLayoutPolicy>
#include <MLabel>
#include <MButton>
#include <MDialog>
#include <MSeparator>
#include <MAction>

CredentialWidgetExample::CredentialWidgetExample(QObject *parent)
{
    Q_UNUSED(parent);
    setStyleName("KeychainPage");
    //% "Keychain"
    setTitle("Test window for the Credential Widget");
}

CredentialWidgetExample::~CredentialWidgetExample()
{
}

void CredentialWidgetExample::createContent()
{
    m_widgetModel = new CredentialWidgetModel();
    m_widgetModel->setUsername("test_username");
    m_widgetModel->setUsernameEditable(true);
    m_widgetModel->setPassword("test_pas");
    m_widgetModel->setFocusOnPasswordField(true);
    m_widgetModel->setCheckboxVisible(true);
    m_widgetModel->setCheckboxPressed(true);
    m_widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible |
                                              CredentialWidgetModel::CaptchaDialogVisible);
    m_widgetModel->setCaptcha(QImage(400, 70, QImage::Format_Mono));
    m_widgetModel->setCaptchaText("captcha_text_test");
    m_widgetModel->setFocusOnCaptchaTextField(true);
    m_widgetModel->setSignInButtonVisible(true);
    m_widgetModel->setNextButtonVisible(true);
    m_widgetModel->setEnabled(true);
    credentialWidget = new CredentialWidget(m_widgetModel);

    MWidget *centralWidget = new MWidget(this);
    MLayout *layout = new MLayout(centralWidget);
    MLinearLayoutPolicy *linearLayout = new MLinearLayoutPolicy(layout, Qt::Vertical);

    MAction *showLoginButton = new MAction(NULL, QLatin1String("Show/hide log in widget"), this);
    showLoginButton->setLocation(MAction::ApplicationMenuLocation);
    addAction(showLoginButton);

    MAction *showCaptchaButton = new MAction(NULL, QLatin1String("Show/hide captcha widget"), this);
    showCaptchaButton->setLocation(MAction::ApplicationMenuLocation);
    addAction(showCaptchaButton);

    MAction *showAllButton = new MAction(NULL, QLatin1String("Show/hide all widgets"), this);
    showAllButton->setLocation(MAction::ApplicationMenuLocation);
    addAction(showAllButton);

    connect(showLoginButton, SIGNAL(triggered()),
            this, SLOT(showLogin()));

    connect(showCaptchaButton, SIGNAL(triggered()),
            this, SLOT(showCaptcha()));

    connect(showAllButton, SIGNAL(triggered()),
            this, SLOT(showAll()));

    linearLayout->addItem(credentialWidget);

    linearLayout->addStretch();
    setCentralWidget(centralWidget);
}

void CredentialWidgetExample::showLogin()
{
    if (m_widgetModel->loginDialogVisible())
        m_widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::CaptchaDialogVisible);
    else
        m_widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible |
                                                  CredentialWidgetModel::CaptchaDialogVisible);
}

void CredentialWidgetExample::showCaptcha()
{
    if (m_widgetModel->captchaDialogVisible())
        m_widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible);
    else
        m_widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible |
                                                  CredentialWidgetModel::CaptchaDialogVisible);
}

void CredentialWidgetExample::showAll()
{
    if (m_widgetModel->captchaDialogVisible() || m_widgetModel->loginDialogVisible())
        m_widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::NoDialogs);
    else
        m_widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible |
                                                  CredentialWidgetModel::CaptchaDialogVisible);
}

