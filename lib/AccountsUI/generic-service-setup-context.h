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

#ifndef ACCOUNTSUI_GENERICSERVICESETUPCONTEXT_H
#define ACCOUNTSUI_GENERICSERVICESETUPCONTEXT_H

#include <AccountsUI/abstract-service-setup-context.h>
#include <AccountsUI/ui-builder.h>
#include <AccountsUI/parser.h>

#include <MWidget>
#include <MWidgetModel>
#include <QDebug>

namespace AccountsUI {

class GenericServiceSetupContextPrivate;

/*!
 * @class GenericServiceSetupContext
 * @headerfile AccountsUI/generic-service-setup-context.h \
 * AccountsUI/GenericServiceSetupContext
 * @brief Default implementation of the service setup context.
 *
 * The GenericServicePlugin is the default implementation of the service
 * setup context.
 */
class ACCOUNTSUI_EXPORT GenericServiceSetupContext :
    public AccountsUI::AbstractServiceSetupContext
{
        Q_OBJECT

    public:
        /*!
         * Class constructor
         */
        GenericServiceSetupContext(Accounts::Account *account,
                                   const Accounts::Service *service,
                                   QObject *parent = 0);

        virtual ~GenericServiceSetupContext();

        /*!
         * Implementation of the store() virtual method.
         * This method goes through all service widgets and writes their
         * current value into the account, under the key described in the XML
         * file.
         */
        void store();

        /*!
         * Adds an account setting.
         * @param object The AccountSetting to be added.
         *
         * This method can be called by the Parser to add a widget to the
         * context.
         */
        void addAccountSetting(AccountSetting *object);

        /*!
         * Create the widget containing all the account's service settings.
         * @param parent A parent for the newly created widget.
         *
         * The default implementation of this method creates a Parser to
         * generate the UI at runtime based on the description found in the
         * service XML file.
         */
        virtual MWidget *widget(QGraphicsItem *parent = 0, bool showAllSettings = true);

        /*!
         * Returns the service XML document.
         */
        QDomDocument domDocument() const
        {
            return service()->domDocument();
        }

        /*!
         * Checks whether the service has mandatory settings.
         */
        virtual bool hasMandatorySettings() const;

        /*!
          * Map which holds key and model associated with the
          * settings widget changes
          */
        QMap<QString, MWidgetModel*> settingsMap;

    protected:
        /*!
         * The Parser which will be used to generate the UI from the service
         * XML file. This variable can be changed by subclasses (remember to
         * delete the existing parser, in order not to leak it).
        */
        Parser *m_parser;

    private:
        GenericServiceSetupContextPrivate *d_ptr;
        Q_DISABLE_COPY(GenericServiceSetupContext)
        Q_DECLARE_PRIVATE(GenericServiceSetupContext)
    };
} // namespace

#endif // ACCOUNTSUI_GENERICSERVICESETUPCONTEXT_H
