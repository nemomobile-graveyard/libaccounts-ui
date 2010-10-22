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

#ifndef ACCOUNTSUI_UIBUILDER_H
#define ACCOUNTSUI_UIBUILDER_H

//libAccountsUI
#include <AccountsUI/common.h>

//Qt
#include <QObject>
#include <QDomDocument>
#include <QGraphicsLayoutItem>

namespace AccountsUI {

class GenericServiceSetupContext;
class Parser;

/*!
 * @class UiBuilder
 * @headerfile AccountsUI/ui-builder.h AccountsUI/Builder
 * @brief Base class for building a new widget or layout.
 */
class ACCOUNTSUI_EXPORT UiBuilder : public QObject
{
    Q_OBJECT
public:
    /*!
     * Class constructor
     */
    UiBuilder(QObject *parent): QObject(parent) {};

    /*!
     * Builds a UI element out of an XML node.
     * @param node The XML node describing the UI to be built.
     * @param context The GenericServiceSetupContext.
     * @param parser The Parser object.
     *
     * The UiBuilder subclass must implement this method to build a UI element
     * (widget or layout) based on the contents of the XML node.
     */
    virtual QGraphicsLayoutItem *ui(const QDomElement &node,
                                    GenericServiceSetupContext *context,
                                    Parser *parser,
                                    bool showAllSettings = true ) = 0;
};

/*!
 * @interface AccountSetting
 * @headerfile AccountsUI/ui-builder.h AccountsUI/Builder
 * @brief Interface for UI widgets containing service settings.
 *
 * This interface should be implemented by all widgets which hold some
 * setting value which should be stored into the account when the user has
 * finished editing the account's service settings. All these widgets must
 * be added to the GenericServiceSetupContext, which will take care of
 * invoking the store() method at the proper time.
 */
class AccountSetting
{
public:
    /*!
     * Store the setting into the account.
     * @param context The GenericServiceSetupContext; you can get the
     * account out of it.
     *
     * This method is called by the setup context when the settings need
     * to be stored into the account.
     */
    virtual void store(GenericServiceSetupContext *context) = 0;
};

}; // namespace

#endif // ACCOUNTSUI_UIBUILDER_H
