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

#ifndef ACCOUNTSUI_GENERICACCOUNTSETUPFORM_H
#define ACCOUNTSUI_GENERICACCOUNTSETUPFORM_H

//project
#include <AccountsUI/genericaccountsetupformmodel.h>
#include <AccountsUI/abstract-account-setup-context.h>

//Meegotouch
#include <MWidgetController>

//Qt
#include <QDomDocument>

class GenericAccountSetupFormPrivate;
using namespace AccountsUI;

class ACCOUNTSUI_EXPORT GenericAccountSetupForm : public MWidgetController
{
    Q_OBJECT
    M_CONTROLLER(GenericAccountSetupForm)

public:
    explicit GenericAccountSetupForm(AbstractAccountSetupContext *context,
                                     QGraphicsItem *parent = 0);

    virtual ~GenericAccountSetupForm();

    QDomDocument domDocument();
    void setDomDocument(const QDomDocument& providerAccountDocument);
    void setUsernameDisplayString(const QString &displaystring);
    const QString username() const;
    const QString password() const;
    const QString providerName() const;
    bool rememberMe() const;

Q_SIGNALS:
    void validateSignal();
    void stopButtonPressed();

public Q_SLOTS:
    void validateCredentials();

private Q_SLOTS:
    void validationOver();
    void errorSlot();

private:
    GenericAccountSetupFormPrivate *d_ptr;
    Q_DISABLE_COPY(GenericAccountSetupForm)
    Q_DECLARE_PRIVATE(GenericAccountSetupForm);
};


#endif // ACCOUNTSUI_GENERICACCOUNTSETUPFORM_H
