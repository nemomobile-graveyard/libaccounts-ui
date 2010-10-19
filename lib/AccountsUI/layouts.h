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

#ifndef ACCOUNTSUI_LAYOUTS_H
#define ACCOUNTSUI_LAYOUTS_H

//M
#include <MLayout>

//Qt
#include <QDebug>

//libAccountsUI
#include <AccountsUI/ui-builder.h>
#include <AccountsUI/parser.h>

using namespace AccountsUI;

// Children are created independently of each other
// and the layout treats them equally ("free-of-context").
// could it build piecewise?
class ContextFreeLayout: public UiBuilder
{
public:
    ContextFreeLayout(QObject *parent):
        UiBuilder(parent) {};
    virtual ~ContextFreeLayout() {
        qDebug() << __PRETTY_FUNCTION__;
    }
protected:
    virtual QList<QGraphicsLayoutItem*> children(const QDomElement &node,
                                                 GenericServiceSetupContext *context,
                                                 Parser *parser,
                                                 bool showAllSettings = true);
    virtual MLayout *layout(const QList<QGraphicsLayoutItem*> &child_list) = 0;
    virtual QGraphicsLayoutItem* ui(const QDomElement &node,
                            GenericServiceSetupContext *context,
                            Parser *parser,
                            bool showAllSettings = true) {
        qDebug() << __PRETTY_FUNCTION__;
        return layout(children(node, context,  parser, showAllSettings));
    };
};

class Layout2Columns : public ContextFreeLayout
{
    Q_OBJECT
public:
    Q_INVOKABLE Layout2Columns(QObject *parent): ContextFreeLayout(parent) {};
protected:
    virtual MLayout *layout(const QList<QGraphicsLayoutItem*> &child_list);
};

class LayoutFlow : public ContextFreeLayout
{
    Q_OBJECT
protected:
    virtual MLayout *layout(const QList<QGraphicsLayoutItem*> &child_list);
public:
    Q_INVOKABLE LayoutFlow(QObject *parent): ContextFreeLayout(parent) {
        qDebug() << __PRETTY_FUNCTION__;
    };
};


class LayoutList : public ContextFreeLayout
{
    Q_OBJECT
public:
    Q_INVOKABLE LayoutList(QObject *parent): ContextFreeLayout(parent) {};
protected:
    virtual MLayout *layout(const QList<QGraphicsLayoutItem*> &child_list);
};

#endif  /* ACCOUNTSUI_LAYOUTS_H */
