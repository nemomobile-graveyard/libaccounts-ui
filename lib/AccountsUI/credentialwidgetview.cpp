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


#include "credentialwidgetview.h"
#include "credentialwidget.h"
#include "credentialimagewidget.h"

//M
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MFlowLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MLabel>
#include <MImageWidget>
#include <MTextEdit>
#include <MLocale>
#include <MContainer>
#include <MButton>
#include <MTextEdit>
#include <MDialog>

//Qt
#include <QDebug>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QImage>

#define ROW_SPACING 30

class CredentialWidgetViewPrivate {

public:
    CredentialWidgetViewPrivate(CredentialWidget *controller)
        : usernameLabel(0),
          usernameTextEdit(0),
          keychainButton(0),
          passwordLabel(0),
          passwordTextEdit(0),
          rememberPasswordLabel(0),
          rememberPasswordSwitch(0),
          captchaLabel(0),
          captchaImage(0),
          captchaRefreshButton(0),
          captchaTextEdit(0),
          captchaImageClicked(false),
          signInButton(0),
          nextButton(0),
          keyChainDialog(0),
          identity(0)
    {
        //layouts and policies init
        mainLayout = new MLayout(controller);
        portraitPolicy = new MGridLayoutPolicy(mainLayout);
        landscapePolicy = new MGridLayoutPolicy(mainLayout);
        portraitPolicy->setSpacing(10);
        landscapePolicy->setSpacing(10);
    }

    ~CredentialWidgetViewPrivate()
    {
    }

    /*
     * these parts are common for all three different
     * variants of accounts-ui dialogs
     */
    MLayout   *mainLayout;

    /*
     * username specific items
     */
    MLabel    *usernameLabel;
    MTextEdit *usernameTextEdit;
    MButton   *keychainButton;

    /*
     * password specific items
     */
    MLabel    *passwordLabel;
    MTextEdit *passwordTextEdit;

    /*
     * remember me switch specific items
     */
    MLabel  *rememberPasswordLabel;
    MButton *rememberPasswordSwitch;

    /*
     * captcha specific items
     */
    MLabel       *captchaLabel;
    CredentialImageWidget *captchaImage;
    MButton      *captchaRefreshButton;
    MTextEdit    *captchaTextEdit;
    bool         captchaImageClicked;

    /*
     * sign in specific button
     */
    MButton* signInButton;

    /*
     * button for going to the next page without authentication
     */
    MButton* nextButton;

    /*
     * layout policies
     */
    MGridLayoutPolicy *portraitPolicy;
    MGridLayoutPolicy *landscapePolicy;

    MDialog *keyChainDialog;

    SignOn::Identity *identity;

public:
    void destroyAllWidgets();
};

void CredentialWidgetViewPrivate::destroyAllWidgets()
{
    if (usernameLabel) {
        delete usernameLabel;
        usernameLabel = NULL;
    }

    if (usernameTextEdit) {
        delete usernameTextEdit;
        usernameTextEdit = NULL;
    }

    if (keychainButton) {
        delete keychainButton;
        keychainButton = NULL;
    }

    if (passwordLabel) {
        delete passwordLabel;
        passwordLabel = NULL;
    }

    if (passwordTextEdit) {
        delete passwordTextEdit;
        passwordTextEdit = NULL;
    }

    if (rememberPasswordLabel) {
        delete rememberPasswordLabel;
        rememberPasswordLabel = NULL;
    }

    if (rememberPasswordSwitch) {
        delete rememberPasswordSwitch;
        rememberPasswordSwitch = NULL;
    }

    if (captchaLabel) {
        delete captchaLabel;
        captchaLabel = NULL;
    }

    if (captchaImage) {
        delete captchaImage;
        captchaImage = NULL;
    }

    if (captchaRefreshButton) {
        delete captchaRefreshButton;
        captchaRefreshButton = NULL;
    }

    if (captchaTextEdit) {
        delete captchaTextEdit;
        captchaTextEdit = NULL;
    }

    if (signInButton) {
        delete signInButton;
        signInButton = NULL;
    }

    if (nextButton) {
        delete nextButton;
        nextButton = NULL;
    }

    captchaImageClicked = false;
}

CredentialWidgetView::CredentialWidgetView(CredentialWidget *controller)
    : MWidgetView(controller),
      d_ptr(new CredentialWidgetViewPrivate(controller))
{
}

CredentialWidgetView::~CredentialWidgetView()
{
}

void CredentialWidgetView::usernameTextEditGainedFocus()
{
    Q_D(CredentialWidgetView);
    static bool firstTime = true;
    if (firstTime)
        d->usernameTextEdit->setCursorPosition(0);

    firstTime = false;
}

void CredentialWidgetView::launchKeychain()
{
    //TODO
}

void CredentialWidgetView::itemPickedSlot(quint32 credentialsId)
{
    Q_D(CredentialWidgetView);

    //dismiss the keychain
    d->keyChainDialog->done(0);

    d->passwordTextEdit->blockSignals(true);
    d->passwordTextEdit->setText("*******");
    d->passwordTextEdit->blockSignals(false);

    d->identity = SignOn::Identity::existingIdentity(credentialsId, this);
    if (d->identity) {
        connect(d->identity, SIGNAL(info(const SignOn::IdentityInfo&)),
                this, SLOT(infoReadySlot(const SignOn::IdentityInfo&)));
        d->identity->queryInfo();
    }
}

void CredentialWidgetView::infoReadySlot(const SignOn::IdentityInfo &info)
{
    Q_D(CredentialWidgetView);

    d->usernameTextEdit->blockSignals(true);
    d->usernameTextEdit->setText(info.userName());
    d->usernameTextEdit->setCursorPosition(0);
    d->usernameTextEdit->blockSignals(false);
}

void CredentialWidgetView::recreateWidgets()
{
    Q_D(CredentialWidgetView);

    if (!d->mainLayout->isEmpty()) {
        for (int i = d->mainLayout->count(); i >= 0; i--)
            d->mainLayout->removeAt(i);
    }

    d->destroyAllWidgets();

    if (model()->loginDialogVisible()) {
        //% "Username"
        d->usernameLabel = new MLabel(qtTrId("qtn_acc_login_username"));
        d->usernameLabel->setStyleName("wgUsernameLabel");

        d->usernameTextEdit = new MTextEdit();
        d->usernameTextEdit->setStyleName("wgUsernameTextEdit");
        d->usernameTextEdit->setContentType(M::EmailContentType); // TO DO url type needs to be supported if we do OpenId

        //% "Password"
        d->passwordLabel = new MLabel(qtTrId("qtn_acc_login_password"));
        d->passwordLabel->setStyleName("wgPasswordLabel");

        d->passwordTextEdit = new MTextEdit();
        d->passwordTextEdit->setStyleName("wgPasswordTextEdit");
        d->passwordTextEdit->setEchoMode(MTextEditModel::Password);

        //% "Remember my password"
        d->rememberPasswordLabel = new MLabel(qtTrId("qtn_acc_login_remember_pw"));
        d->rememberPasswordLabel->setStyleName("wgRememberPasswordLabel");

        d->rememberPasswordSwitch = new MButton();
        d->rememberPasswordSwitch->setStyleName("wgRememberPasswordSwitch");
        d->rememberPasswordSwitch->setViewType(MButton::switchType);
        d->rememberPasswordSwitch->setCheckable(true);

        d->usernameTextEdit->setReadOnly(!model()->usernameEditable());
        d->usernameTextEdit->setText(model()->username());
        d->passwordTextEdit->setText(model()->password());

        connect(d->passwordTextEdit, SIGNAL(lostFocus(Qt::FocusReason)),
                this, SLOT(refreshPasswordInModel()));

        connect(d->usernameTextEdit, SIGNAL(lostFocus(Qt::FocusReason)),
                this, SLOT(refreshUsernameInModel()));

        connect(d->usernameTextEdit, SIGNAL(widgetClicked()),
                this, SLOT(usernameTextEditGainedFocus()));

        if (model()->checkboxVisible()) {
            d->rememberPasswordSwitch->setChecked(model()->checkboxPressed());
            connect(d->rememberPasswordSwitch, SIGNAL(toggled(bool)),
                    this, SLOT(refreshCheckboxStateInModel(bool)));
        }
    }

    if (model()->captchaDialogVisible()) {
        //% "Enter characters you see in the picture below"
        d->captchaLabel = new MLabel(qtTrId("qtn_acc_login_captcha_info"));
        d->captchaLabel->setStyleName("wgCaptchaLabel");

        d->captchaImage = new CredentialImageWidget;
        d->captchaImage->setStyleName("wgCaptchaImage");

        //TODO: change after specs clarification
        QSize size(365, 70);
        d->captchaImage->setImage(model()->captcha().scaled(size, Qt::IgnoreAspectRatio));

        d->captchaRefreshButton = new MButton();
        d->captchaRefreshButton->setStyleName("wgCaptchaRefreshButton");
        d->captchaRefreshButton->setIconID("icon-m-toolbar-refresh");

        d->captchaTextEdit = new MTextEdit();
        d->captchaTextEdit->setStyleName("wgCaptchaTextEdit");
        d->captchaTextEdit->setText(model()->captchaText());

        connect(d->captchaImage, SIGNAL(widgetClicked()),
                this, SLOT(captchaImageClicked()));
        connect(d->captchaTextEdit, SIGNAL(textChanged()),
                this, SLOT(refreshCaptchaTextInModel()));
    }

    if (d->signInButton == NULL) {
        //% "Sign in"
        d->signInButton = new MButton(qtTrId("qtn_acc_auth_dial_button"));
        d->signInButton->setStyleName("wgSignInButton");

        connect(d->signInButton, SIGNAL(clicked()),
                        model(), SIGNAL(signInClicked()));
    }

    if (d->nextButton == NULL) {
        //% "Next"
        d->nextButton = new MButton(qtTrId("qtn_acc_next_button"));
        d->nextButton->setStyleName("wgNextButton");

        connect(d->nextButton, SIGNAL(clicked()),
                        model(), SIGNAL(nextClicked()));
    }

    d->captchaImageClicked = false;
    setEnabled(model()->enabled());
}

void CredentialWidgetView::setupModel()
{
    MWidgetView::setupModel();

    recreateWidgets();
    updateMainLayout();
}

void CredentialWidgetView::applyStyle()
{
    MWidgetView::applyStyle();
}

void CredentialWidgetView::updateData(const QList <const char *> &modifications)
{
    Q_D(CredentialWidgetView);

    MWidgetView::updateData(modifications);
    const char *member;

    foreach(member, modifications) {
        if (member == CredentialWidgetModel::Username) {
            if (d->usernameTextEdit)
                d->usernameTextEdit->setText(model()->username());
        }
        else if (member == CredentialWidgetModel::UsernameEditable) {
            if (d->usernameTextEdit)
                d->usernameTextEdit->setReadOnly(model()->usernameEditable());
        }
        else if (member == CredentialWidgetModel::Password) {
            if (d->passwordTextEdit)
                d->passwordTextEdit->setText(model()->password());
        }
        else if (member == CredentialWidgetModel::FocusOnPasswordField) {
            if (d->passwordTextEdit)
                d->passwordTextEdit->setFocus(Qt::ActiveWindowFocusReason);
        }
        else if (member == CredentialWidgetModel::CheckboxVisible) {
            if (d->rememberPasswordSwitch)
                d->rememberPasswordSwitch->show();
        }
        else if (member == CredentialWidgetModel::CheckboxPressed) {
            if (d->rememberPasswordSwitch)
                d->rememberPasswordSwitch->setChecked(model()->checkboxPressed());
        }
        else if (member == CredentialWidgetModel::Captcha) {
            if (d->captchaImage) {
                //TODO: cahnge after specs clarification
                QSize size(200, 80);
                d->captchaImage->setImage(model()->captcha().scaled(size, Qt::IgnoreAspectRatio));
                d->captchaImageClicked = false;
            }
        }
        else if (member == CredentialWidgetModel::CaptchaText) {
            if (d->captchaTextEdit)
                d->captchaTextEdit->setText(model()->captchaText());
        }
        else if (member == CredentialWidgetModel::FocusOnCaptchaTextField) {
            if (d->captchaTextEdit)
                d->captchaTextEdit->setFocus(Qt::ActiveWindowFocusReason);
        }
        else if (member == CredentialWidgetModel::Enabled) {
            setEnabled(model()->enabled());
        }
    }
}

void CredentialWidgetView::setEnabled(bool isWidgetEnabled)
{
    Q_D(CredentialWidgetView);

    if (d->usernameLabel)
        d->usernameLabel->setEnabled(isWidgetEnabled);

    if (d->usernameTextEdit)
        d->usernameTextEdit->setEnabled(isWidgetEnabled);

    if (d->keychainButton)
        d->keychainButton->setEnabled(isWidgetEnabled);

    if (d->passwordLabel)
        d->passwordLabel->setEnabled(isWidgetEnabled);

    if (d->passwordTextEdit)
        d->passwordTextEdit->setEnabled(isWidgetEnabled);

    if (d->rememberPasswordLabel)
        d->rememberPasswordLabel->setEnabled(isWidgetEnabled);

    if (d->rememberPasswordSwitch)
        d->rememberPasswordSwitch->setEnabled(isWidgetEnabled);

    if (d->captchaLabel)
        d->captchaLabel->setEnabled(isWidgetEnabled);

    if (d->captchaImage)
        d->captchaImage->setEnabled(isWidgetEnabled);

    if (d->captchaRefreshButton)
        d->captchaRefreshButton->setEnabled(isWidgetEnabled);

    if (d->captchaTextEdit)
        d->captchaTextEdit->setEnabled(isWidgetEnabled);

    if (d->signInButton)
        d->signInButton->setEnabled(isWidgetEnabled);

    if (d->nextButton)
        d->nextButton->setEnabled(isWidgetEnabled);
}

void CredentialWidgetView::refreshUsernameInModel()
{
    Q_D(CredentialWidgetView);
    model()->blockSignals(true);
    model()->setUsername(d->usernameTextEdit->text());
    model()->blockSignals(false);
}

void CredentialWidgetView::refreshPasswordInModel()
{
    Q_D(CredentialWidgetView);
    model()->blockSignals(true);
    model()->setPassword(d->passwordTextEdit->text());
    model()->blockSignals(false);
}

void CredentialWidgetView::refreshCaptchaTextInModel()
{
    Q_D(CredentialWidgetView);
    model()->blockSignals(true);
    model()->setCaptchaText(d->captchaTextEdit->text());
    model()->blockSignals(false);
}

void CredentialWidgetView::refreshCheckboxStateInModel(bool checked)
{
    model()->setCheckboxPressed(checked);
}

void CredentialWidgetView::captchaImageClicked()
{
    Q_D(CredentialWidgetView);

    if (!d->captchaImageClicked && d->captchaImage) {
        d->captchaImageClicked = true;
        model()->captchaClick();
    }
}

void CredentialWidgetView::configureWithCaptchaAndLogin()
{
    Q_D(CredentialWidgetView);

    //portrait mode
    d->portraitPolicy->addItem(d->usernameLabel,0,0);
    d->portraitPolicy->addItem(d->usernameTextEdit,1,0,1,2);
    d->portraitPolicy->addItem(d->passwordLabel,2,0);
    d->portraitPolicy->addItem(d->passwordTextEdit,3,0,1,2);

    if (model()->checkboxVisible()) {
        d->portraitPolicy->addItem(d->rememberPasswordLabel,4,0);
        d->portraitPolicy->addItem(d->rememberPasswordSwitch,4,1);
        d->portraitPolicy->addItem(d->captchaLabel,5,0,1,2);
        d->portraitPolicy->addItem(d->captchaImage,6,0,Qt::AlignLeft);
        d->portraitPolicy->addItem(d->captchaRefreshButton,6,1,Qt::AlignRight);
        d->portraitPolicy->addItem(d->captchaTextEdit,7,0,1,2);
        if (model()->signInButtonVisible())
            d->portraitPolicy->addItem(d->signInButton,8,0,1,2);
        if (model()->nextButtonVisible())
            d->portraitPolicy->addItem(d->nextButton,9,0,1,2);
    } else {
        d->portraitPolicy->addItem(d->captchaLabel,4,0,1,2);
        d->portraitPolicy->addItem(d->captchaImage,5,0,Qt::AlignLeft);
        d->portraitPolicy->addItem(d->captchaRefreshButton,5,1,Qt::AlignRight);
        d->portraitPolicy->addItem(d->captchaTextEdit,6,0,1,2);
        if (model()->signInButtonVisible())
            d->portraitPolicy->addItem(d->signInButton,7,0,1,2);
        if (model()->nextButtonVisible())
            d->portraitPolicy->addItem(d->nextButton,7,0,1,2);
    }

    //landscape mode
    d->landscapePolicy->addItem(d->usernameLabel,0,0);
    d->landscapePolicy->addItem(d->passwordLabel,0,2,1,2);
    d->landscapePolicy->addItem(d->usernameTextEdit,1,0,1,2);
    d->landscapePolicy->addItem(d->passwordTextEdit,1,2,1,2);

    if (model()->checkboxVisible()) {
        d->landscapePolicy->addItem(d->rememberPasswordLabel,2,0,1,2);
        d->landscapePolicy->addItem(d->rememberPasswordSwitch,2,3,1,1);
        d->landscapePolicy->addItem(d->captchaLabel,3,0,1,4);
        d->landscapePolicy->addItem(d->captchaImage,4,0,1,1,Qt::AlignLeft);
        d->landscapePolicy->addItem(d->captchaRefreshButton,4,3,1,1,Qt::AlignRight);
        d->landscapePolicy->addItem(d->captchaTextEdit,5,0,1,4);

        if (model()->signInButtonVisible() && model()->nextButtonVisible()) {
            d->landscapePolicy->addItem(d->signInButton,6,0,1,2);
            d->landscapePolicy->addItem(d->nextButton,6,2,1,2);
        } else {
            if (model()->signInButtonVisible())
                d->landscapePolicy->addItem(d->signInButton,6,0,1,4);
            else
                d->landscapePolicy->addItem(d->nextButton,6,0,1,4);
            d->landscapePolicy->setRowAlignment(6,Qt::AlignHCenter);
        }
    } else {
        d->landscapePolicy->addItem(d->captchaLabel,2,0,1,4);
        d->landscapePolicy->addItem(d->captchaImage,3,0,1,1,Qt::AlignLeft);
        d->landscapePolicy->addItem(d->captchaRefreshButton,3,3,1,1,Qt::AlignRight);
        d->landscapePolicy->addItem(d->captchaTextEdit,4,0,1,4);

        if (model()->signInButtonVisible() && model()->nextButtonVisible()) {
            d->landscapePolicy->addItem(d->signInButton,5,0,1,2);
            d->landscapePolicy->addItem(d->nextButton,5,2,1,2);
        } else {
            if (model()->signInButtonVisible())
                d->landscapePolicy->addItem(d->signInButton,5,0,1,4);
            else
                d->landscapePolicy->addItem(d->nextButton,5,0,1,4);
            d->landscapePolicy->setRowAlignment(5,Qt::AlignHCenter);
        }
    }
}

void CredentialWidgetView::configureWithCaptcha()
{
    Q_D(CredentialWidgetView);

    //portrait mode
    d->portraitPolicy->addItem(d->captchaLabel,0,0,1,2);
    d->portraitPolicy->addItem(d->captchaImage,1,0,Qt::AlignLeft);
    d->portraitPolicy->addItem(d->captchaRefreshButton,1,1,Qt::AlignRight);
    d->portraitPolicy->addItem(d->captchaTextEdit,2,0,1,2);

    //lanscape mode
    d->landscapePolicy->addItem(d->captchaLabel,0,0,1,4);
    d->landscapePolicy->addItem(d->captchaImage,1,0,1,1,Qt::AlignLeft);
    d->landscapePolicy->addItem(d->captchaRefreshButton,1,3,1,1,Qt::AlignRight);
    d->landscapePolicy->addItem(d->captchaTextEdit,2,0,1,4);
}

void CredentialWidgetView::configureWithLogin()
{
    Q_D(CredentialWidgetView);

    //portrait mode
    d->portraitPolicy->addItem(d->usernameLabel,0,0);
    d->portraitPolicy->addItem(d->usernameTextEdit,1,0,1,2);
    d->portraitPolicy->addItem(d->passwordLabel,2,0);
    d->portraitPolicy->addItem(d->passwordTextEdit,3,0,1,2);

    if (model()->checkboxVisible()) {
        d->portraitPolicy->addItem(d->rememberPasswordLabel,4,0);
        d->portraitPolicy->addItem(d->rememberPasswordSwitch,4,1);
        if (model()->signInButtonVisible() && model()->nextButtonVisible()) {
            d->portraitPolicy->addItem(d->signInButton,5,0,1,2);
            d->portraitPolicy->addItem(d->nextButton,6,0,1,2);
        } else {
            if (model()->signInButtonVisible())
                d->portraitPolicy->addItem(d->signInButton,5,0,1,2);
            else
                d->portraitPolicy->addItem(d->nextButton,5,0,1,2);
        }
    } else {
        if (model()->signInButtonVisible() && model()->nextButtonVisible()) {
            d->portraitPolicy->addItem(d->signInButton,4,0,1,2);
            d->portraitPolicy->addItem(d->nextButton,5,0,1,2);
        } else {
            if (model()->signInButtonVisible())
                d->portraitPolicy->addItem(d->signInButton,4,0,1,2);
            else
                d->portraitPolicy->addItem(d->nextButton,4,0,1,2);
        }
    }

    //landscape mode
    d->landscapePolicy->addItem(d->usernameLabel,0,0);
    d->landscapePolicy->addItem(d->passwordLabel,0,2);
    d->landscapePolicy->addItem(d->usernameTextEdit,1,0,1,2);
    d->landscapePolicy->addItem(d->passwordTextEdit,1,2,1,2);

    if (model()->checkboxVisible()) {
        d->landscapePolicy->addItem(d->rememberPasswordLabel,2,0,1,3);
        d->landscapePolicy->addItem(d->rememberPasswordSwitch,2,3,1,1,Qt::AlignRight);

        if (model()->signInButtonVisible() && model()->nextButtonVisible()) {
            d->landscapePolicy->addItem(d->signInButton,3,0,1,2);
            d->landscapePolicy->addItem(d->nextButton,3,2,1,2);
        } else {
            if (model()->signInButtonVisible())
                d->landscapePolicy->addItem(d->signInButton,3,0,1,4);
            else
                d->landscapePolicy->addItem(d->nextButton,3,0,1,4);
            d->landscapePolicy->setRowAlignment(3,Qt::AlignHCenter);
        }
    } else {
        if (model()->signInButtonVisible() && model()->nextButtonVisible()) {
            d->landscapePolicy->addItem(d->signInButton,2,0,1,2);
            d->landscapePolicy->addItem(d->nextButton,2,2,1,2);
        } else {
            if (model()->signInButtonVisible())
                d->landscapePolicy->addItem(d->signInButton,2,0,1,4);
            else
                d->landscapePolicy->addItem(d->nextButton,2,0,1,4);
            d->landscapePolicy->setRowAlignment(2,Qt::AlignHCenter);
        }
    }
}

void CredentialWidgetView::configureWithButtonsOnly()
{
    Q_D(CredentialWidgetView);

    //landscape && portrait mode
    if (model()->signInButtonVisible() && model()->nextButtonVisible()) {
        d->landscapePolicy->addItem(d->signInButton,0,0,1,2);
        d->landscapePolicy->addItem(d->nextButton,0,2,1,2);
        d->portraitPolicy->addItem(d->signInButton,0,0);
        d->portraitPolicy->addItem(d->nextButton,1,0);
    } else {
        if (model()->signInButtonVisible()) {
            d->landscapePolicy->addItem(d->signInButton,0,0,1,4);
            d->portraitPolicy->addItem(d->signInButton,0,0);
        } else {
            d->landscapePolicy->addItem(d->nextButton,0,0,1,4);
            d->portraitPolicy->addItem(d->nextButton,0,0);
        }
        d->landscapePolicy->setRowAlignment(0,Qt::AlignHCenter);
    }
}

void CredentialWidgetView::updateMainLayout()
{
    Q_D(CredentialWidgetView);

    if (model()->captchaDialogVisible() &&
        model()->loginDialogVisible())
        configureWithCaptchaAndLogin();
    else if (model()->captchaDialogVisible())
        configureWithCaptcha();
    else if (model()->loginDialogVisible())
        configureWithLogin();
    else
        configureWithButtonsOnly();

    d->landscapePolicy->setColumnStretchFactor(0,49);
    d->landscapePolicy->setColumnStretchFactor(1,1);
    d->landscapePolicy->setColumnStretchFactor(2,49);
    d->landscapePolicy->setColumnStretchFactor(3,1);

    d->portraitPolicy->setColumnStretchFactor(0,99);
    d->portraitPolicy->setColumnStretchFactor(1,1);

    d->landscapePolicy->setRowSpacing(1,ROW_SPACING);
    d->landscapePolicy->setRowSpacing(2,ROW_SPACING);
    d->landscapePolicy->setRowSpacing(5,ROW_SPACING);

    d->portraitPolicy->setRowSpacing(1,ROW_SPACING);
    d->portraitPolicy->setRowSpacing(3,ROW_SPACING);
    d->portraitPolicy->setRowSpacing(4,ROW_SPACING);
    d->portraitPolicy->setRowSpacing(7,ROW_SPACING);

    d->mainLayout->setPortraitPolicy(d->portraitPolicy);
    d->mainLayout->setLandscapePolicy(d->landscapePolicy);
}

M_REGISTER_VIEW_NEW(CredentialWidgetView, CredentialWidget);
