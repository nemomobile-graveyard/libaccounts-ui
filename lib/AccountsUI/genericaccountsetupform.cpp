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

#include "genericaccountsetupform.h"
#include "genericaccountsetupformview.h"

//accounts-qt lib

#include <Accounts/Account>

//Meegotouch
#include <MTheme>
#include <MWidgetCreator>
#include <MLibrary>

M_LIBRARY

//Qt
#include <QDebug>

using namespace Accounts;

class GenericAccountSetupFormPrivate
{
public:
    GenericAccountSetupFormPrivate()
            : context(0)
            , accountValidationInProgress(false)
    {}
    ~GenericAccountSetupFormPrivate()
    {}

    AbstractAccountSetupContext *context;
    bool accountValidationInProgress;
};

GenericAccountSetupForm::GenericAccountSetupForm(AbstractAccountSetupContext *context,
                                               QGraphicsItem *parent)
    : MWidgetController(new GenericAccountSetupFormModel(), parent),
      d_ptr(new GenericAccountSetupFormPrivate())
{
    Q_D(GenericAccountSetupForm);

    d->context = context;
    MWidgetView *defaultWidgetView = MTheme::view(this);

    Q_ASSERT_X(defaultWidgetView,
               "no MWidgetView",
               "There is no available WidgetView class for this widget - "
               "is there an existing and correct conf file?" );
    setView(defaultWidgetView);

     //to check that the MWidgetController Constructor created a model
    Q_ASSERT(model());

    connect(this, SIGNAL(validateSignal()), d->context, SLOT(validate()));

    connect(d->context, SIGNAL(error(AccountsUI::ErrorCode, const QString&)), SLOT(validationOver()));
    connect(d->context, SIGNAL(validated()), SLOT(validationOver()));
    connect(d->context,
            SIGNAL(error(AccountsUI::ErrorCode, const QString&)),
            SLOT(errorSlot()));
}

GenericAccountSetupForm::~GenericAccountSetupForm()
{
    delete d_ptr;
}

void GenericAccountSetupForm::setDomDocument(const QDomDocument& providerAccountDocument)
{
    //this method make deepcopy through the string - so we can write
    model()->setProviderAccountDocumentAsString(providerAccountDocument.toString());
}

QDomDocument GenericAccountSetupForm::domDocument()
{
    QDomDocument providerAccountDocument;
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!providerAccountDocument.setContent(model()->providerAccountDocumentAsString(),
                                            true,
                                            &errorStr,
                                            &errorLine,
                                            &errorColumn) ) {
        QString topic("DOM error reading of account provider domdocument");
        QString message("Parse error at line %1, column %2:\n%3");
        message = message.arg(errorLine).arg(errorColumn).arg(errorStr);
        Q_ASSERT_X(false, topic.toAscii(), message.toAscii());
        qWarning() << topic << message;
    }
    return providerAccountDocument;
}

const QString GenericAccountSetupForm::username() const
{
    return model()->username();
}

const QString GenericAccountSetupForm::password() const
{
    return model()->password();
}

const QString GenericAccountSetupForm::providerName() const
{
    return model()->providerName();
}

bool GenericAccountSetupForm::rememberMe() const
{
    return model()->rememberMe();
}

void GenericAccountSetupForm::validateCredentials()
{
    Q_D(GenericAccountSetupForm);

    if (d->accountValidationInProgress == true)
        return;

    disableForm();
    d->accountValidationInProgress = true;
    emit validateSignal();
}

void GenericAccountSetupForm::validationOver()
{
    Q_D(GenericAccountSetupForm);
    d->accountValidationInProgress = false;
    enableForm();
}

void GenericAccountSetupForm::errorSlot()
{
    /*
     * TODO: define
     * actual functionality for this
     * */

    Q_D(GenericAccountSetupForm);
    d->accountValidationInProgress = false;
    enableForm();
}

void GenericAccountSetupForm::setEnabledForm(bool enable)
{
    /*
     * TODO: add enabling/disabling of account setup form elements
     * */
    if (enable) {
        //nothing yet
    } else {
        //still nothing yet
    }
}

M_REGISTER_WIDGET_NO_CREATE(GenericAccountSetupForm);
