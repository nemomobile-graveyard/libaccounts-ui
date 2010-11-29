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

#ifndef ACCOUNTSUI_WIDGETS_H
#define ACCOUNTSUI_WIDGETS_H

//Qt
#include <QDebug>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QMetaObject>

//M
#include <MButton>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MTextEdit>
#include <MLabel>

//Accounts
#include <Accounts/Account>

//libAccountsUI
#include <AccountsUI/generic-service-setup-context.h>
#include <AccountsUI/parser.h>
#include <AccountsUI/ui-builder.h>

using namespace Accounts;
using namespace AccountsUI;

// basic widget
class ParameterText: public UiBuilder, public AccountSetting
{
    Q_OBJECT

private:
    QString key;

public:
    Q_INVOKABLE ParameterText (QObject *parent)
        : UiBuilder(parent), key(QString::null), editor(0)
    {};

    virtual ~ParameterText () {
    }

    virtual void store(GenericServiceSetupContext *context) {
        context->account()->setValue(key, editor->text());
    }

    virtual QGraphicsLayoutItem* ui(const QDomElement &node, GenericServiceSetupContext *context,
                                    Parser *parser, bool showAllSettings = true);

private slots:

protected:
    MTextEdit *editor;
};

class ParameterInteger: public UiBuilder, public AccountSetting
{
    Q_OBJECT

private:
    QString key;

protected:
    MTextEdit *editor;

    virtual void store(GenericServiceSetupContext *context) {
        context->account()->setValue(key,
                                     editor->text().toInt());
    }

public:
    Q_INVOKABLE ParameterInteger (QObject *parent)
    : UiBuilder(parent), key(QString::null), editor(0) {};

    virtual ~ParameterInteger () {
    };

    virtual QGraphicsLayoutItem* ui(const QDomElement &node, GenericServiceSetupContext *context,
                                    Parser *parser, bool showAllSettings = true);

};


class ParameterBoolean: public UiBuilder, public AccountSetting
{
    Q_OBJECT

private:
    QString key;

protected:
    MButton *button;
    virtual void store(GenericServiceSetupContext *context)
        {
            context->account()->setValue(key,button->isChecked());
        }
public:
    Q_INVOKABLE ParameterBoolean (QObject *parent)
    : UiBuilder(parent), key(QString::null), button(0) {};

    virtual ~ParameterBoolean () {
    };

    virtual QGraphicsLayoutItem* ui(const QDomElement &node, GenericServiceSetupContext *context,
                                    Parser *parser, bool showAllSettings = true);
};

#endif  /* ACCOUNTSUI_WIDGETS_H */
