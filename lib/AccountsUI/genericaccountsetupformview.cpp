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

//M
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MLocale>
#include <MButton>
#include <MTextEdit>
#include <MInfoBanner>
#include <MContentItem>
#include <MSeparator>
#include <MLabel>

//Qt
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

class GenericAccountSetupFormViewPrivate
{
public:
    GenericAccountSetupFormViewPrivate(GenericAccountSetupFormView *publicObject)
        : widgetModel(0),
          isInDomDocumentUpdate(false),
          q_ptr(publicObject)
    {
        controller = 0;
        mainLayoutPolicy = 0;
        registerNewLink = QString();
        authDomainSeparator = QString();
        authDomainDefault = QString();
    }

    ~GenericAccountSetupFormViewPrivate()
    {}

    void createUiFromXml(const QDomDocument &providerAccountDocument);

public:
    GenericAccountSetupForm *controller;
    MLinearLayoutPolicy *mainLayoutPolicy;
    MLayout *mainLayout;
    QMap<MWidget*, QDomElement> formWidgetDomElementMap;
    QDomDocument providerAccountDocument;
    CredentialWidgetModel *widgetModel;
    bool isInDomDocumentUpdate;
    QString registerNewLink;
    QString authDomainSeparator;
    QString authDomainDefault;

protected:
    Q_DECLARE_PUBLIC(GenericAccountSetupFormView);
    GenericAccountSetupFormView *q_ptr;
};

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
    QDomElement signUpLink = root.firstChildElement("sign-up-link");
    QString providerName = root.firstChildElement("name").text();
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
    MContentItem *providerInfoItem =
        new MContentItem(MContentItem::IconAndSingleTextLabel, controller);
    providerInfoItem->setObjectName("pluginProviderName");
    providerInfoItem->setTitle(providerName);
    providerInfoItem->setImageID(providerIconId);

    MLabel *descriptionLabel = new MLabel(descriptionText);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setWrapMode(QTextOption::WordWrap);

    // Credentials widget
    if (widgetModel) {
        delete widgetModel;
        widgetModel = NULL;
    }

    MSeparator *separator = new MSeparator();
    separator->setOrientation(Qt::Horizontal);

    widgetModel = new CredentialWidgetModel();
    widgetModel->setDialogsVisabilityConfig(CredentialWidgetModel::LoginDialogVisible);
    widgetModel->setSignInButtonVisible(true);
    CredentialWidget *credentialWidget = new CredentialWidget(widgetModel);
    if (!authDomainSeparator.isEmpty() || !authDomainDefault.isEmpty())
        credentialWidget->setUsername(authDomainSeparator + authDomainDefault);
    QObject::connect(widgetModel, SIGNAL(signInClicked()), q_ptr , SLOT(signIn()));
    QObject::connect(widgetModel, SIGNAL(modified(QList<const char*>)),
                     q_ptr, SLOT(updateModel(QList<const char*>)));

    // add the widgets to the layout
    mainLayoutPolicy->addItem(providerInfoItem);
    mainLayoutPolicy->addItem(descriptionLabel);
    mainLayoutPolicy->addItem(separator);
    mainLayoutPolicy->addItem(credentialWidget);
    mainLayoutPolicy->addStretch(3);

    if (!registerNewLink.isEmpty()) {

        //% "Don't have a %1 account yet?"
        MLabel *questionLabel = new MLabel(QString("Don't have a %1 account yet?").arg(providerName));
        questionLabel->setAlignment(Qt::AlignCenter);
        questionLabel->setObjectName("AccountsPrimaryInfoLabel");

        //% "Get one here"
        QString link("Get one here!<a href=\"%1\"></a>");
        MLabel *subscribeLabel = new MLabel(link.arg(registerNewLink));
        subscribeLabel->setTextFormat(Qt::RichText);
        subscribeLabel->setAlignment(Qt::AlignCenter);
        subscribeLabel->setObjectName("AccountsSecondaryInfoLabel");
        QObject::connect(subscribeLabel, SIGNAL(clicked()),
                         controller, SLOT(registerNew()));

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
    qDebug() << __PRETTY_FUNCTION__;
    Q_D(GenericAccountSetupFormView);

    MWidgetView::updateData(modifications);

    foreach(const char *member, modifications) {
        if (!qstrcmp(member, "providerAccountDocumentAsString") &&
            d->isInDomDocumentUpdate == false)
        {
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
        }
    }
}

void GenericAccountSetupFormView::setupModel()
{
    qDebug() << __PRETTY_FUNCTION__;
    MWidgetView::setupModel();
}

void GenericAccountSetupFormView::signIn()
{
    qDebug()<<__PRETTY_FUNCTION__;
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
