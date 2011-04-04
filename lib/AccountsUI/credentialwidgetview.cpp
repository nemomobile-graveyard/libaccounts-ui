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
#include <MInputMethodState>

//Qt
#include <QDebug>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QImage>

#define ROW_SPACING 30
const int BinaryTextVariantSeparator = 0x9c;

class CredentialWidgetViewPrivate {

public:
    CredentialWidgetViewPrivate(CredentialWidget *controller)
        : informativeNoteLabel(0),
          usernameLabel(0),
          usernameTextEdit(0),
          keychainButton(0),
          passwordLabel(0),
          passwordTextEdit(0),
          forgotPasswordLabel(0),
          rememberPasswordLabel(0),
          rememberPasswordSwitch(0),
          captchaLabel(0),
          captchaImage(0),
          captchaRefreshButton(0),
          captchaTextEdit(0),
          captchaImageClicked(false),
          signInButton(0),
          nextButton(0),
          cancelButton(0),
          keyChainDialog(0),
          identity(0)
    {
        //layouts and policies init
        mainLayout = new MLayout(controller);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        portraitPolicy = new MGridLayoutPolicy(mainLayout);
        landscapePolicy = new MGridLayoutPolicy(mainLayout);
        portraitPolicy->setSpacing(0);
        landscapePolicy->setSpacing(0);
    }

    ~CredentialWidgetViewPrivate()
    {
    }

    /*
     * these parts are common for all three different
     * variants of accounts-ui dialogs
     */
    MLayout *mainLayout;


    /*
     * label for displaying informative texts
     */
    MLabel *informativeNoteLabel;

    /*
     * username specific items
     */
    MLabel *usernameLabel;
    MTextEdit *usernameTextEdit;
    MButton *keychainButton;

    /*
     * password specific items
     */
    MLabel *passwordLabel;
    MTextEdit *passwordTextEdit;

    /*
     * forgot password specific items
     */
    MLabel *forgotPasswordLabel;

    /*
     * remember me switch specific items
     */
    MLabel *rememberPasswordLabel;
    MButton *rememberPasswordSwitch;

    /*
     * captcha specific items
     */
    MLabel *captchaLabel;
    CredentialImageWidget *captchaImage;
    MButton *captchaRefreshButton;
    MTextEdit *captchaTextEdit;
    bool captchaImageClicked;

    /*
     * sign in specific button
     */
    MButton *signInButton;

    /*
     * button for going to the next page without authentication
     */
    MButton *nextButton;

    /*
     * button for cancelation
     */
    MButton *cancelButton;

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
    if (informativeNoteLabel) {
        delete informativeNoteLabel;
        informativeNoteLabel = NULL;
    }

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

    if (forgotPasswordLabel) {
        delete forgotPasswordLabel;
        forgotPasswordLabel = NULL;
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

    if (cancelButton) {
        delete cancelButton;
        cancelButton = NULL;
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
    int id = MInputMethodState::instance()->registerAttributeExtension();
    d->usernameTextEdit->attachToolbar(id);
    MInputMethodState::instance()->setExtendedAttribute
            (id, "/keys", "actionKey", "label", qtTrId("qtn_comm_next"));
    connect(d->usernameTextEdit, SIGNAL(returnPressed()),
            this, SLOT(passwordTextEditSetFocus()));
}

void CredentialWidgetView::passwordTextEditGainedFocus()
{
    Q_D(CredentialWidgetView);
    int id = MInputMethodState::instance()->registerAttributeExtension();
    d->passwordTextEdit->attachToolbar(id);
    MInputMethodState::instance()->setExtendedAttribute(id, "/keys", "actionKey","label",
                                                        qtTrId("qtn_comm_ok"));
    connect(d->passwordTextEdit, SIGNAL(returnPressed()),
            this, SLOT(closeVKB()));
}

void CredentialWidgetView::closeVKB()
{
    Q_D(CredentialWidgetView);
    MInputMethodState::closeSoftwareInputPanel();
    QGraphicsItem *passwordItem = static_cast<QGraphicsItem *>(d->passwordTextEdit);
    passwordItem->clearFocus();
}

void CredentialWidgetView::passwordTextEditSetFocus()
{
    Q_D(CredentialWidgetView);
    QGraphicsItem *passwordItem = static_cast<QGraphicsItem *>(d->passwordTextEdit);
    passwordItem->setFocus();
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
        for (int i = d->mainLayout->count() - 1; i >= 0; i--)
            d->mainLayout->removeAt(i);
    }

    d->destroyAllWidgets();

    if (model()->loginDialogVisible()) {

        if (!model()->informativeNoteText().isEmpty()) {
            d->informativeNoteLabel = new MLabel(model()->informativeNoteText());
            d->informativeNoteLabel->setStyleName("CommonBodyTextInverted");
            d->informativeNoteLabel->setWordWrap(true);
            d->informativeNoteLabel->setWrapMode(QTextOption::WordWrap);
            d->informativeNoteLabel->setAlignment(Qt::AlignCenter);
        }

        //% "Username"
        d->usernameLabel = new MLabel(qtTrId("qtn_acc_login_username"));
        d->usernameLabel->setStyleName("CommonFieldLabelInverted");

        d->usernameTextEdit = new MTextEdit();
        d->usernameTextEdit->setStyleName("CommonSingleInputFieldLabeledInverted");
        d->usernameTextEdit->setContentType(M::EmailContentType); // TO DO url type needs to be supported if we do OpenId

        //% "Password"
        d->passwordLabel = new MLabel(qtTrId("qtn_acc_login_password"));
        d->passwordLabel->setStyleName("CommonFieldLabelInverted");

        d->passwordTextEdit = new MTextEdit();
        d->passwordTextEdit->setStyleName("CommonSingleInputFieldLabeledInverted");
        d->passwordTextEdit->setEchoMode(MTextEditModel::Password);

        if (model()->forgotPassword().isEmpty()) {
            //% "Forgot my password"
            QString link("<a href=\"%1\"> " + qtTrId("qtn_acc_forgot_password") + "! </a>");
            link = link.left(link.indexOf(QChar(BinaryTextVariantSeparator)));
            d->forgotPasswordLabel = new MLabel(link.arg(model()->forgotPasswordUrl()));
        } else {
            d->forgotPasswordLabel = new MLabel(model()->forgotPassword());
            d->forgotPasswordLabel->setWordWrap(true);
            d->forgotPasswordLabel->setWrapMode(QTextOption::WordWrap);
        }
        d->forgotPasswordLabel->setStyleName("CommonFieldLabelInverted");
        d->forgotPasswordLabel->setTextFormat(Qt::RichText);
        d->forgotPasswordLabel->setAlignment(Qt::AlignCenter);
        d->forgotPasswordLabel->setStyleName("CommonFieldLabelInverted");
        d->forgotPasswordLabel->setObjectName("wgForgotPasswordLabel");

        //% "Remember my password"
        d->rememberPasswordLabel = new MLabel(qtTrId("qtn_acc_login_remember_pw"));
        d->rememberPasswordLabel->setStyleName("CommonFieldLabelInverted");

        d->rememberPasswordSwitch = new MButton();
        d->rememberPasswordSwitch->setObjectName("wgRememberPasswordSwitch");
        d->rememberPasswordSwitch->setStyleName("CommonSwitchInverted");
        d->rememberPasswordSwitch->setViewType(MButton::switchType);
        d->rememberPasswordSwitch->setCheckable(true);

        d->usernameTextEdit->setReadOnly(!model()->usernameEditable());
        d->usernameTextEdit->setText(model()->username());
        d->passwordTextEdit->setText(model()->password());

        connect(d->passwordTextEdit, SIGNAL(gainedFocus(Qt::FocusReason)),
                this, SLOT(passwordTextEditGainedFocus()));

        connect(d->passwordTextEdit, SIGNAL(lostFocus(Qt::FocusReason)),
                this, SLOT(refreshPasswordInModel()));

        connect(d->usernameTextEdit, SIGNAL(lostFocus(Qt::FocusReason)),
                this, SLOT(refreshUsernameInModel()));

        connect(d->usernameTextEdit, SIGNAL(gainedFocus (Qt::FocusReason)),
                this, SLOT(usernameTextEditGainedFocus()));

        connect(d->forgotPasswordLabel, SIGNAL(linkActivated(QString)),
                this, SLOT(forgotPasswordClicked(QString)));

        if (model()->checkboxVisible()) {
            d->rememberPasswordSwitch->setChecked(model()->checkboxPressed());
            connect(d->rememberPasswordSwitch, SIGNAL(toggled(bool)),
                    this, SLOT(refreshCheckboxStateInModel(bool)));
        }
    }

    if (model()->captchaDialogVisible()) {
        //% "Enter characters you see in the picture below"
        d->captchaLabel = new MLabel(qtTrId("qtn_acc_login_captcha_info"));
        d->captchaLabel->setStyleName("CommonFieldLabelInverted");

        d->captchaImage = new CredentialImageWidget;
        d->captchaImage->setStyleName("wgCaptchaImage");

        //TODO: change after specs clarification
        QSize size(365, 70);
        d->captchaImage->setImage(model()->captcha().scaled(size, Qt::IgnoreAspectRatio));

        d->captchaRefreshButton = new MButton();
        d->captchaRefreshButton->setStyleName("CommonSingleButtonInverted");
        d->captchaRefreshButton->setIconID("icon-m-toolbar-refresh");

        d->captchaTextEdit = new MTextEdit();
        d->captchaTextEdit->setStyleName("CommonSingleInputFieldLabeledInverted");
        d->captchaTextEdit->setText(model()->captchaText());

        connect(d->captchaImage, SIGNAL(widgetClicked()),
                this, SLOT(captchaImageClicked()));
        connect(d->captchaTextEdit, SIGNAL(textChanged()),
                this, SLOT(refreshCaptchaTextInModel()));
    }

    if (model()->signInButtonVisible()) {
        Q_ASSERT(d->signInButton == 0);
        //% "CONNECT"
        d->signInButton = new MButton(qtTrId("qtn_acc_auth_dial_button"));
        d->signInButton->setStyleName("CommonSingleButtonInverted");
        d->signInButton->setObjectName("wgSignInButton");

        connect(d->signInButton, SIGNAL(clicked()),
                        model(), SIGNAL(signInClicked()));
    }

    if (model()->nextButtonVisible()) {
        Q_ASSERT(d->nextButton == 0);
        //% "Next"
        d->nextButton = new MButton(qtTrId("qtn_acc_next_button"));
        d->nextButton->setStyleName("CommonSingleButtonInverted");

        connect(d->nextButton, SIGNAL(clicked()),
                        model(), SIGNAL(nextClicked()));
    }

    if (model()->cancelButtonVisible()) {
        Q_ASSERT(d->cancelButton == 0);
        //% "Cancel"
        d->cancelButton = new MButton(qtTrId("qtn_comm_cancel"));
        d->cancelButton->setStyleName("CommonSingleButtonInverted");

        connect(d->cancelButton, SIGNAL(clicked()),
                        model(), SIGNAL(cancelClicked()));
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
        else if (member == CredentialWidgetModel::InformativeNoteText) {
            if (d->informativeNoteLabel)
                d->informativeNoteLabel->setText(model()->informativeNoteText());
        }
    }
}

void CredentialWidgetView::setEnabled(bool isWidgetEnabled)
{
    Q_D(CredentialWidgetView);

    if (d->informativeNoteLabel)
        d->informativeNoteLabel->setEnabled(isWidgetEnabled);

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

    if (d->forgotPasswordLabel)
        d->forgotPasswordLabel->setEnabled(isWidgetEnabled);

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

    if (d->cancelButton)
        d->cancelButton->setEnabled(isWidgetEnabled);
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

void CredentialWidgetView::forgotPasswordClicked(QString link)
{
    model()->forgotPasswordClick(link);
}

void CredentialWidgetView::configureWithCaptchaAndLogin()
{
    Q_D(CredentialWidgetView);

    int row = 0;
    //portrait mode
    d->portraitPolicy->addItem(d->informativeNoteLabel,row,0);
    row++;
    d->portraitPolicy->addItem(d->usernameLabel,row,0);
    row++;
    d->portraitPolicy->addItem(d->usernameTextEdit,row,0,1,2);
    row++;
    d->portraitPolicy->addItem(d->passwordLabel,row,0);
    row++;
    d->portraitPolicy->addItem(d->passwordTextEdit,row,0,1,2);

    if (model()->forgotPasswordVisible()) {
        row++;
        d->portraitPolicy->addItem(d->forgotPasswordLabel,row,0);
    }

    if (model()->checkboxVisible()) {
        row++;
        d->portraitPolicy->addItem(d->rememberPasswordLabel,row,0);
        d->portraitPolicy->addItem(d->rememberPasswordSwitch,row,1);
    }

    row++;
    d->portraitPolicy->addItem(d->captchaLabel,row,0,1,2);
    row++;
    d->portraitPolicy->addItem(d->captchaImage,row,0,Qt::AlignLeft);
    d->portraitPolicy->addItem(d->captchaRefreshButton,row,1,Qt::AlignRight);
    row++;
    d->portraitPolicy->addItem(d->captchaTextEdit,row,0,1,2);

    if (model()->signInButtonVisible()) {
        row++;
        d->portraitPolicy->addItem(d->signInButton,row,0,1,2);
    }

    if (model()->cancelButtonVisible()) {
        row++;
        d->portraitPolicy->addItem(d->cancelButton,row,0,1,2);
    }

    if (model()->nextButtonVisible()) {
        row++;
        d->portraitPolicy->addItem(d->nextButton,row,0,1,2);
    }

    row = 0;
    //landscape mode
    d->landscapePolicy->addItem(d->informativeNoteLabel,row,0,1,2);
    d->landscapePolicy->setRowAlignment(row,Qt::AlignHCenter);
    row++;
    d->landscapePolicy->addItem(d->usernameLabel,row,0);
    row++;
    d->landscapePolicy->addItem(d->usernameTextEdit,row,0);
    row--;
    d->landscapePolicy->addItem(d->passwordLabel,row,1);
    row++;
    d->landscapePolicy->addItem(d->passwordTextEdit,row,1);

    if (model()->forgotPasswordVisible()) {
        row++;
        d->landscapePolicy->addItem(d->forgotPasswordLabel,row,0);
    }

    if (model()->checkboxVisible()) {
        row++;
        d->landscapePolicy->addItem(d->rememberPasswordLabel,row,0,1,2);
        d->landscapePolicy->addItem(d->rememberPasswordSwitch,row,3,1,1);
    }

    row++;
    d->landscapePolicy->addItem(d->captchaLabel,row,0,1,4);
    row++;
    d->landscapePolicy->addItem(d->captchaImage,row,0,1,1,Qt::AlignLeft);
    d->landscapePolicy->addItem(d->captchaRefreshButton,row,3,1,1,Qt::AlignRight);
    row++;
    d->landscapePolicy->addItem(d->captchaTextEdit,row,0,1,4);

    if (model()->signInButtonVisible()) {
        row++;
        d->landscapePolicy->addItem(d->signInButton,row,0,1,2);
        d->landscapePolicy->setRowAlignment(row,Qt::AlignHCenter);
    }

    if (model()->cancelButtonVisible()) {
        row++;
        d->landscapePolicy->addItem(d->cancelButton,row,0,1,2);
        d->landscapePolicy->setRowAlignment(row,Qt::AlignHCenter);
    }

    if (model()->nextButtonVisible()) {
        row++;
        d->landscapePolicy->addItem(d->nextButton,row,0,1,2);
        d->landscapePolicy->setRowAlignment(row,Qt::AlignHCenter);
    }

}

void CredentialWidgetView::configureWithCaptcha()
{
    Q_D(CredentialWidgetView);

    int row = 0;
    //portrait mode
    d->portraitPolicy->addItem(d->captchaLabel,row,0,1,2);
    row++;
    d->portraitPolicy->addItem(d->captchaImage,row,0,Qt::AlignLeft);
    d->portraitPolicy->addItem(d->captchaRefreshButton,row,1,Qt::AlignRight);
    row++;
    d->portraitPolicy->addItem(d->captchaTextEdit,row,0,1,2);

    row = 0;
    //lanscape mode
    d->landscapePolicy->addItem(d->captchaLabel,row,0,1,4);
    row++;
    d->landscapePolicy->addItem(d->captchaImage,row,0,1,1,Qt::AlignLeft);
    d->landscapePolicy->addItem(d->captchaRefreshButton,row,3,1,1,Qt::AlignRight);
    row++;
    d->landscapePolicy->addItem(d->captchaTextEdit,row,0,1,4);
}

void CredentialWidgetView::configureWithLogin()
{
    Q_D(CredentialWidgetView);

    int row = 0;
    //portrait mode
    d->portraitPolicy->addItem(d->informativeNoteLabel,row,0);
    row++;
    d->portraitPolicy->addItem(d->usernameLabel,row,0);
    d->portraitPolicy->setRowSpacing(row,0);
    row++;
    d->portraitPolicy->addItem(d->usernameTextEdit,row,0,1,2);
    row++;
    d->portraitPolicy->addItem(d->passwordLabel,row,0);
    d->portraitPolicy->setRowSpacing(row,0);
    row++;
    d->portraitPolicy->addItem(d->passwordTextEdit,row,0,1,2);

    if (model()->forgotPasswordVisible()) {
        row++;
        d->portraitPolicy->addItem(d->forgotPasswordLabel,row,0);
    }

    if (model()->checkboxVisible()) {
        row++;
        d->portraitPolicy->addItem(d->rememberPasswordLabel,row,0);
        d->portraitPolicy->addItem(d->rememberPasswordSwitch,row,1);
    }

    if (model()->signInButtonVisible()) {
        row++;
        d->portraitPolicy->addItem(d->signInButton,row,0,1,2,Qt::AlignHCenter);
    }

    if (model()->cancelButtonVisible()) {
        row++;
        d->portraitPolicy->addItem(d->cancelButton,row,0,1,2,Qt::AlignHCenter);
    }

    if (model()->nextButtonVisible()) {
        row++;
        d->portraitPolicy->addItem(d->nextButton,row,0,1,2,Qt::AlignHCenter);
    }

    row = 0;
    //landscape mode
    d->landscapePolicy->addItem(d->informativeNoteLabel,row,0,1,2);
    d->landscapePolicy->setRowAlignment(row,Qt::AlignHCenter);
    row++;
    d->landscapePolicy->addItem(d->usernameLabel,row,0);
    row++;
    d->landscapePolicy->addItem(d->usernameTextEdit,row,0);
    row--;
    d->landscapePolicy->addItem(d->passwordLabel,row,1);
    row++;
    d->landscapePolicy->addItem(d->passwordTextEdit,row,1);

    if (model()->forgotPasswordVisible()) {
        row++;
        d->landscapePolicy->addItem(d->forgotPasswordLabel,row,0,1,2,Qt::AlignHCenter);
        d->landscapePolicy->setRowAlignment(row,Qt::AlignHCenter);
    }

    if (model()->checkboxVisible()) {
        row++;
        d->landscapePolicy->addItem(d->rememberPasswordLabel,row,0,1,3);
        d->landscapePolicy->addItem(d->rememberPasswordSwitch,row,3,1,1,Qt::AlignRight);
    }

    if (model()->signInButtonVisible()) {
        row++;
        d->landscapePolicy->addItem(d->signInButton,row,0,1,2,Qt::AlignHCenter);
    }

    if (model()->cancelButtonVisible()) {
        row++;
        d->landscapePolicy->addItem(d->cancelButton,row,0,1,2,Qt::AlignHCenter);
    }

    if (model()->nextButtonVisible()) {
        row++;
        d->landscapePolicy->addItem(d->nextButton,row,0,1,2,Qt::AlignHCenter);
    }
}

void CredentialWidgetView::configureWithButtonsOnly()
{
    Q_D(CredentialWidgetView);

    int row = 0;
    //landscape && portrait mode
    if (model()->signInButtonVisible()) {
        d->landscapePolicy->addItem(d->signInButton,row,0,1,2);
        d->portraitPolicy->addItem(d->signInButton,row,0);
    }
    if (model()->cancelButtonVisible()) {
        row++;
        d->landscapePolicy->addItem(d->cancelButton,row,0,1,2);
        d->portraitPolicy->addItem(d->cancelButton,row,0);
    }
    d->landscapePolicy->setRowAlignment(0,Qt::AlignHCenter);
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

    d->landscapePolicy->setColumnStretchFactor(0,1);
    d->landscapePolicy->setColumnStretchFactor(1,1);

    d->portraitPolicy->setColumnStretchFactor(0,99);
    d->portraitPolicy->setColumnStretchFactor(1,1);

    d->mainLayout->setPortraitPolicy(d->portraitPolicy);

    if(model()->forcedPortraitMode())
        d->mainLayout->setLandscapePolicy(d->portraitPolicy);
    else
        d->mainLayout->setLandscapePolicy(d->landscapePolicy);
}

M_REGISTER_VIEW_NEW(CredentialWidgetView, CredentialWidget);
