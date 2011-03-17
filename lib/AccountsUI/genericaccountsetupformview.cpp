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

//project
#include "credentialwidget.h"
#include "genericaccountsetupform.h"
#include "genericaccountsetupformview.h"
#include "common.h"
#include "accountsmanagersingleton.h"
#include "provider-plugin-process.h"

//M
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MLocale>
#include <MButton>
#include <MTextEdit>
#include <MInfoBanner>
#include <MBasicListItem>
#include <MSeparator>
#include <MLabel>
#include <MProgressIndicator>
#include <MImageWidget>
#include <MMessageBox>

//Qt
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

using namespace Accounts;
const int BinaryTextVariantSeparator = 0x9c;

class GenericAccountSetupFormListItem: public MBasicListItem
{
public:
    GenericAccountSetupFormListItem(QGraphicsWidget *parent)
            : MBasicListItem(MBasicListItem::IconWithTitle, parent)
    {
        setStyleName("CommonBasicListItemInverted");
        titleLabelWidget()->setStyleName("GenericAccountSetupFormListItemStyle");
        setEnabled(false);
    }

    ~GenericAccountSetupFormListItem()
    {}
};

class GenericAccountSetupFormViewPrivate
{
public:
    GenericAccountSetupFormViewPrivate(GenericAccountSetupFormView *publicObject)
        : widgetModel(0),
          isInDomDocumentUpdate(false),
          q_ptr(publicObject)
    {
        controller = NULL;
        mainLayoutPolicy = NULL;
        mainLayout = NULL;
        separator = NULL;
        descriptionLabel = NULL;
        providerInfoItem = NULL;
        questionLabel = NULL;
        subscribeLabel = NULL;
        progressIndicator = NULL;
        connectingLabel = NULL;
        stopButton = NULL;
        widgetModel = NULL;
        credentialWidget = NULL;

        registerNewLink = QString();
        authDomainSeparator = QString();
        authDomainDefault = QString();
        provider = QString();
    }

    ~GenericAccountSetupFormViewPrivate()
    {}

    void createUiFromXml(const QDomDocument &providerAccountDocument);
    void showCredentialWidgetAndHideProgress();
    void hideCredentialWidgetAndShowProgress();

public:
    GenericAccountSetupForm *controller;
    MLinearLayoutPolicy *mainLayoutPolicy;
    MLayout *mainLayout;
    MSeparator *separator;
    MLabel *descriptionLabel;
    GenericAccountSetupFormListItem *providerInfoItem;
    CredentialWidgetModel *widgetModel;
    CredentialWidget *credentialWidget;
    MLabel *questionLabel;
    MLabel *subscribeLabel;
    MProgressIndicator *progressIndicator;
    MLabel *connectingLabel;
    MButton *stopButton;

    QMap<MWidget*, QDomElement> formWidgetDomElementMap;
    QDomDocument providerAccountDocument;
    bool isInDomDocumentUpdate;
    QString registerNewLink;
    QString authDomainSeparator;
    QString authDomainDefault;
    QString provider;

protected:
    Q_DECLARE_PUBLIC(GenericAccountSetupFormView);
    GenericAccountSetupFormView *q_ptr;
};

void GenericAccountSetupFormViewPrivate::showCredentialWidgetAndHideProgress()
{
    if (mainLayoutPolicy) {
        for (int i = mainLayoutPolicy->count() - 1; i >= 0; i--)
            mainLayoutPolicy->removeAt(i);
    }

    if (mainLayout) {
        for (int i = mainLayout->count() - 1; i >= 0; i--)
            mainLayout->removeAt(i);
    }

    if (mainLayoutPolicy) {
        delete mainLayoutPolicy;
        mainLayoutPolicy = NULL;
    }

    if (connectingLabel) {
        delete connectingLabel;
        connectingLabel = NULL;
    }

    if (stopButton) {
        delete stopButton;
        stopButton = NULL;
    }

    mainLayoutPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Vertical);

    mainLayoutPolicy->addItem(providerInfoItem, Qt::AlignTop);
    mainLayoutPolicy->addItem(descriptionLabel, Qt::AlignTop);
    mainLayoutPolicy->addItem(separator, Qt::AlignTop);
    mainLayoutPolicy->addItem(credentialWidget, Qt::AlignTop);
    mainLayoutPolicy->addStretch();
    mainLayoutPolicy->addItem(questionLabel, Qt::AlignCenter | Qt::AlignBottom);
    mainLayoutPolicy->addItem(subscribeLabel, Qt::AlignCenter | Qt::AlignBottom);
}

void GenericAccountSetupFormViewPrivate::hideCredentialWidgetAndShowProgress()
{
    if (mainLayoutPolicy) {
        for (int i = mainLayoutPolicy->count() - 1; i >= 0; i--)
            mainLayoutPolicy->removeAt(i);
    }

    if (mainLayout) {
        for (int i = mainLayout->count() - 1; i >= 0; i--)
            mainLayout->removeAt(i);
    }

    if (mainLayoutPolicy) {
        delete mainLayoutPolicy;
        mainLayoutPolicy = NULL;
    }

    mainLayoutPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Vertical);

    mainLayoutPolicy->addItem(providerInfoItem, Qt::AlignTop);
    mainLayoutPolicy->addItem(descriptionLabel, Qt::AlignTop);
    mainLayoutPolicy->addItem(separator, Qt::AlignTop);
    progressIndicator = new MProgressIndicator(NULL, MProgressIndicator::spinnerType);
    progressIndicator->setUnknownDuration(true);
    progressIndicator->setStyleName("CommonLargeSpinnerInverted");

    //% "Connecting"
    connectingLabel = new MLabel(qtTrId("qtn_acc_connecting"));
    connectingLabel->setStyleName("ConnectingLabel");
    connectingLabel->setObjectName("wgGenericAccountSetupFormViewLabel");
    connectingLabel->setAlignment(Qt::AlignCenter);

    //% "STOP"
    stopButton = new MButton(qtTrId("qtn_comm_stop"));
    stopButton->setStyleName("CommonSingleButtonInverted");
    stopButton->setObjectName("wgGenericAccountSetupFormViewButton");
    QObject::connect(stopButton, SIGNAL(clicked()),
                     controller, SIGNAL(stopButtonPressed()));

    mainLayoutPolicy->addStretch();
    mainLayoutPolicy->addStretch(10);
    mainLayoutPolicy->addItem(progressIndicator, Qt::AlignCenter);
    mainLayoutPolicy->addStretch(2);
    mainLayoutPolicy->addItem(connectingLabel, Qt::AlignCenter);
    mainLayoutPolicy->addStretch(2);
    mainLayoutPolicy->addItem(stopButton, Qt::AlignCenter);
    mainLayoutPolicy->addStretch();
    mainLayoutPolicy->addStretch(10);
}

void GenericAccountSetupFormViewPrivate::createUiFromXml(const QDomDocument &aProviderAccountDocument)
{
    Q_Q(GenericAccountSetupFormView);
    MLocale locale;
    locale.installTrCatalog("signon-ui");
    MLocale::setDefault(locale);

    // xml file that describes the ui elements for the provider
    providerAccountDocument = aProviderAccountDocument;
    QDomElement root = aProviderAccountDocument.documentElement();
    QDomElement descriptionElement = root.firstChildElement("description");
    QDomElement providerIcon = root.firstChildElement("icon");
    QDomElement catalog = root.firstChildElement("translations");
    QDomElement signUpLink = root.firstChildElement("sign-up-link");
    provider = root.attribute("id");

    if (!catalog.text().isEmpty()) {
        MLocale locale;
        locale.installTrCatalog(catalog.text());
        MLocale::setDefault(locale);
    }
    QString providerName = qtTrId(root.firstChildElement("name").text().toLatin1());
    if (q->model() != NULL) {
        q->model()->setProviderName(providerName);
    }
    QString descriptionText = descriptionElement.text();
    QString providerIconId = providerIcon.text();
    registerNewLink = signUpLink.text();

    QDomElement node = root.firstChildElement("account-setup").firstChildElement();
    while (!node.isNull()) {
        if (node.tagName() == "authdomain") {
            /* we select the first occurrence*/
            authDomainSeparator = node.attribute("separator");
            authDomainDefault = node.attribute("default_domain");
            break;
        }
        node = node.nextSiblingElement();
    }

    // Provider info widgets
    providerInfoItem = new GenericAccountSetupFormListItem(controller);
    providerInfoItem->setObjectName("wgGenericAccountSetupProviderInfoTitle");
    providerInfoItem->setTitle(providerName);
    providerInfoItem->imageWidget()->setImage(providerIconId);

    descriptionLabel = new MLabel(qtTrId(descriptionText.toLatin1()));
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setWrapMode(QTextOption::WordWrap);
    descriptionLabel->setStyleName("CommonBodyTextInverted");

    // Credentials widget
    if (widgetModel) {
        delete widgetModel;
        widgetModel = NULL;
    }

    separator = new MSeparator();
    separator->setOrientation(Qt::Horizontal);
    separator->setStyleName("CommonItemDividerInverted");

    widgetModel = new CredentialWidgetModel();
    widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible);
    widgetModel->setSignInButtonVisible(true);

    QObject::connect(widgetModel, SIGNAL(signInClicked()), q_ptr , SLOT(signIn()));
    QObject::connect(widgetModel, SIGNAL(modified(QList<const char*>)),
                     q_ptr, SLOT(updateModel(QList<const char*>)));

    credentialWidget = new CredentialWidget(widgetModel);
    if (!authDomainSeparator.isEmpty() || !authDomainDefault.isEmpty())
        credentialWidget->setUsername(authDomainSeparator + qtTrId(authDomainDefault.toLatin1()));

    // add the widgets to the layout
    mainLayoutPolicy->addItem(providerInfoItem);
    mainLayoutPolicy->addItem(descriptionLabel);
    mainLayoutPolicy->addItem(separator);
    mainLayoutPolicy->addItem(credentialWidget);
    mainLayoutPolicy->addStretch(3);

    if (!registerNewLink.isEmpty()) {

        //% "Don't have a %1 account yet?"
        questionLabel = new MLabel(qtTrId("qtn_acc_login_new_to_x").arg(providerName));
        questionLabel->setStyleName("CommonBodyTextInverted");
        questionLabel->setAlignment(Qt::AlignCenter);
        questionLabel->setWordWrap(true);
        questionLabel->setWrapMode(QTextOption::WordWrap);
        questionLabel->setObjectName("AccountsPrimaryInfoLabel");

        //% "Get one here"
        QString link("<a href=\"%1\"> " + qtTrId("qtn_acc_login_register_here") + "! </a>");
        /* fetch the first lenght variant as qt doesnt provide
            support for length variants for rich text */
        link = link.left(link.indexOf(QChar(BinaryTextVariantSeparator)));
        subscribeLabel = new MLabel(link.arg(registerNewLink));
        subscribeLabel->setTextFormat(Qt::RichText);
        subscribeLabel->setAlignment(Qt::AlignCenter);
        subscribeLabel->setObjectName("AccountsSecondaryInfoLabel");
        QObject::connect(subscribeLabel, SIGNAL(linkActivated(QString)),
                              q_ptr, SLOT(registerNew()));

        mainLayoutPolicy->addItem(questionLabel, Qt::AlignCenter);
        mainLayoutPolicy->addItem(subscribeLabel, Qt::AlignCenter);
    }
}

GenericAccountSetupFormView::GenericAccountSetupFormView(GenericAccountSetupForm *controller)
    : MWidgetView(controller)
    , d_ptr(new GenericAccountSetupFormViewPrivate(this))
{
    Q_ASSERT(controller);
    Q_D(GenericAccountSetupFormView);
    d->controller = controller;

    // layouts
    d->mainLayout = new MLayout();
    d->mainLayoutPolicy = new MLinearLayoutPolicy(d->mainLayout, Qt::Vertical);

    controller->setLayout(d->mainLayout);
}

GenericAccountSetupFormView::~GenericAccountSetupFormView()
{
    delete d_ptr;
}

void GenericAccountSetupFormView::updateDomDocument()
{
    qDebug()<<__PRETTY_FUNCTION__;
    Q_D(GenericAccountSetupFormView);

    Q_ASSERT(qobject_cast<MWidget*>(sender()));
    MWidget *formElement = qobject_cast<MWidget*>(sender());

    QDomElement domElement = d->formWidgetDomElementMap.value(formElement);

    if (qobject_cast<MTextEdit*>(sender())) {
        MTextEdit *textEdit = qobject_cast<MTextEdit*>(sender());
        domElement.setAttribute("value", textEdit->text());
    }

    if (qobject_cast<MButton*>(sender())) {
        MButton *button = qobject_cast<MButton*>(sender());
        button->setStyleName("button");
        domElement.setAttribute("value", QVariant(button->isChecked()).toString());
    }

    d->isInDomDocumentUpdate = true;

    // save the updated dom document back to the model
    model()->setProviderAccountDocumentAsString(d->providerAccountDocument.toString());
    d->isInDomDocumentUpdate = false;
}

void GenericAccountSetupFormView::updateData(const QList<const char*> &modifications)
{
    Q_D(GenericAccountSetupFormView);

    MWidgetView::updateData(modifications);

    foreach(const char *member, modifications) {
        if (!qstrcmp(member, "providerAccountDocumentAsString") &&
            d->isInDomDocumentUpdate == false) {
            QDomDocument providerAccountDocument;
            QString errorStr;
            int errorLine;
            int errorColumn;

            if (!providerAccountDocument.setContent(
                 model()->providerAccountDocumentAsString(),
                 true, &errorStr, &errorLine, &errorColumn)) {
                 Q_ASSERT_X(false, "DOM reading of provider account document",
                                  QString("Parse error at line %1, column %2:\n%3")
                                  .arg(errorLine)
                                  .arg(errorColumn)
                                  .arg(errorStr).toAscii());
            }
            d->createUiFromXml(providerAccountDocument);
        } else if (!qstrcmp(member, "credentialWidgetEnabled")) {
            if (model()->credentialWidgetEnabled()) {
                d->showCredentialWidgetAndHideProgress();
            } else {
                d->hideCredentialWidgetAndShowProgress();
            }
        }
    }
}

void GenericAccountSetupFormView::setupModel()
{
    MWidgetView::setupModel();
}

void GenericAccountSetupFormView::signIn()
{
    Q_D(GenericAccountSetupFormView);

    if (d->widgetModel->username().isEmpty()) {
        //% "Fill in username"
        showInfoBanner(qtTrId("qtn_acc_fill_in_username_infobanner"));
        return;
    }

    if (d->widgetModel->password().isEmpty()) {
        //% "Fill in password"
        showInfoBanner(qtTrId("qtn_acc_fill_in_password_infobanner"));
        return;
    }

    if (!d->authDomainSeparator.isEmpty() &&
        !d->widgetModel->username().contains(d->authDomainSeparator)) {
        //% "Fill in username@example.com"
        showInfoBanner(qtTrId("qtn_acc_fill_in_username_with_domain_infobanner"));
        return;
    }

    AccountIdList idList = AccountsManager::instance()->accountList();
    foreach (Accounts::AccountId id, idList) {
        Accounts::Account *account = AccountsManager::instance()->account(id);
        if ((d->widgetModel->username() == account->displayName()) &&
            (d->provider == account->providerName())) {
            MMessageBox queryBox(qtTrId("qtn_acc_account_exists"), qtTrId("qtn_acc_tune_it_in_accounts"), 0);
            MButton *continueButton = new MButton(qtTrId("qtn_comm_command_continue"));
            MButton *cancelButton = new MButton(qtTrId("qtn_comm_cancel"));
            queryBox.addButton(continueButton->model());
            queryBox.addButton(cancelButton->model());
            queryBox.exec();
            if (queryBox.clickedButton() == continueButton->model())
                ProviderPluginProcess::instance()->setReturnToAccountsList(true);
            else if (queryBox.clickedButton() == cancelButton->model())
                ProviderPluginProcess::instance()->quit();
        }
    }

    model()->setRememberMe(d->widgetModel->checkboxPressed());
    model()->setUsername(d->widgetModel->username());
    model()->setPassword(d->widgetModel->password());

    d->controller->validateCredentials();
}

void GenericAccountSetupFormView::updateModel(QList<const char*> modifications)
{
    Q_D(GenericAccountSetupFormView);

    foreach(const char *member, modifications) {
        if (!qstrcmp(member, "username")) {
            model()->setUsername(d->widgetModel->username());
        } else if (!qstrcmp(member, "password")) {
            model()->setPassword(d->widgetModel->password());
        }
     }
}

void GenericAccountSetupFormView::registerNew()
{
    Q_D(GenericAccountSetupFormView);
    if (!QDesktopServices::openUrl(QUrl(d->registerNewLink)))
    qWarning() << Q_FUNC_INFO << "Unable to open web browser";
}

// bind view and controller together
M_REGISTER_VIEW_NEW(GenericAccountSetupFormView, GenericAccountSetupForm)
