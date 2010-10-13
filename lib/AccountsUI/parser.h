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

#ifndef ACCOUNTSUI_PARSER_H
#define ACCOUNTSUI_PARSER_H

#include <QDomElement>
#include <MLayout>

#include <QHash>
#include <QMetaObject>

#include <AccountsUI/ui-builder.h>

namespace AccountsUI {

class GenericServiceSetupContext;
class ParserPrivate;

/*!
 * @class Parser
 * @headerfile AccountsUI/parser.h AccountsUI/Parser
 * @brief Generator of UI elements from XML description.
 */
class ACCOUNTSUI_EXPORT Parser : QObject
{
    Q_OBJECT
public:
    Parser();

    virtual ~Parser();

    /*!
     * Parses a tree of the XML UI description.
     * @param node The root of the XML tree.
     * @param context The service setup context.
     *
     * This virtual method is first called by parseDocument() with the
     * QDomElement representing the XML tree to parse. It should generate
     * the required UI widgets and layouts, and return them.
     */
    virtual QGraphicsLayoutItem *parse(const QDomElement &node,
                                       GenericServiceSetupContext *context, bool showAllSettings = true);

    /*!
     * Parses the XML UI description
     * @param doc The XML document of the service.
     * @param context The service setup context.
     *
     * This method is called to generate the UI elements for editing the
     * service settings.
     */
    QGraphicsLayoutItem *parseDocument(const QDomDocument &doc,
                                       GenericServiceSetupContext *context, bool showAllSettings = true);

    /*!
     * Registers a new widget or layout.
     * @param tag The XML tag name that identifies the widget or layout.
     * @param object A QMetaObject for the widget or layout.
     *
     * Call this method to extend the parser so that it supports a new widget
     * type. The object must be a subclass of AccountsUI::UiBuilder.
     */
    void registerWidgetMeta(const QString &tag, const QMetaObject *object);
    // no way to unregister, so plugins cannot unload.
private:
    ParserPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Parser);
};

} // namespace

#endif // ACCOUNTSUI_PARSER_H
