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

#ifndef ACCOUNTSUI_GENERICACCOUNTSETUPFORMVIEW_H
#define ACCOUNTSUI_GENERICACCOUNTSETUPFORMVIEW_H

//project
#include <AccountsUI/genericaccountsetupform.h>
#include <AccountsUI/genericaccountsetupformstyle.h>

//Meegotouch
#include <MWidgetView>

class GenericAccountSetupFormViewPrivate;

class GenericAccountSetupFormView : public MWidgetView
{
    Q_OBJECT
    M_VIEW(GenericAccountSetupFormModel, GenericAccountSetupFormStyle)

public:
    GenericAccountSetupFormView(GenericAccountSetupForm *controller);
    virtual ~GenericAccountSetupFormView();

protected:
    virtual void updateData(const QList<const char*> &modifications);
    virtual void setupModel();

    /**
      * protected d pointer for later use in dirived classes
      */
    GenericAccountSetupFormViewPrivate *d_ptr;

private slots:
    void updateDomDocument();
    void signIn();
    void updateModel(QList<const char*>);
    void registerNew();
    void forgotPassword(QString link);

private:
    Q_DISABLE_COPY(GenericAccountSetupFormView)
    Q_DECLARE_PRIVATE(GenericAccountSetupFormView);
};

#endif // ACCOUNTSUI_GENERICACCOUNTSETUPFORMVIEW_H
