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

#include "generic-service-setup-context.h"

#include <QDebug>

#include "settings-widgets.h"
#include "layouts.h"
#include "parser.h"

using namespace Accounts;

typedef QList<AccountSetting*> AccountSettingList;

namespace AccountsUI {

class GenericServiceSetupContextPrivate
{
public:
    GenericServiceSetupContextPrivate()
        : accountSettings()
    {
    }

    AccountSettingList accountSettings;
};

GenericServiceSetupContext::GenericServiceSetupContext(Account *account,
                                                       const Service *service,
                                                       QObject *parent)
    : AbstractServiceSetupContext(account, service, parent)
    , m_parser (new Parser)
    , d_ptr(new GenericServiceSetupContextPrivate())
{
    m_parser->registerWidgetMeta("string", &ParameterText::staticMetaObject);
    m_parser->registerWidgetMeta("password", &ParameterText::staticMetaObject);
    m_parser->registerWidgetMeta("email", &ParameterText::staticMetaObject);
    m_parser->registerWidgetMeta("integer", &ParameterInteger::staticMetaObject);
    m_parser->registerWidgetMeta("checkbox", &ParameterBoolean::staticMetaObject);

    m_parser->registerWidgetMeta("flow", &LayoutFlow::staticMetaObject);
    m_parser->registerWidgetMeta("2columns", &Layout2Columns::staticMetaObject);
    m_parser->registerWidgetMeta("linear", &LayoutList::staticMetaObject);
}

GenericServiceSetupContext::~GenericServiceSetupContext()
{
    Q_D(GenericServiceSetupContext);

    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }

    delete d;
}

void GenericServiceSetupContext::store()
{
    Q_D(GenericServiceSetupContext);
    qDebug() << Q_FUNC_INFO;

    account()->selectService(service());
    foreach(AccountSetting *storable, d->accountSettings) {
        storable->store(this);
    }
    emit stored();
}


MWidget *GenericServiceSetupContext::widget(QGraphicsItem *parent, bool showAllSettings)
{
    qDebug() << Q_FUNC_INFO;

    QGraphicsLayoutItem *item = NULL;
    item = m_parser->parseDocument(domDocument(), this, showAllSettings);

    // the caller wants a MWidget:
    if (MLayout *layout = dynamic_cast<MLayout*> (item)) {
        MWidget *widget = new MWidget (parent);
        layout->setParentLayoutItem(widget); // fixme: maybe not necessary.
        widget->setLayout(layout);
        return widget;
    } else if (MWidget *single_widget = dynamic_cast<MWidget*> (item)) {
        return single_widget;
    } else {
        qDebug () << "The parser produced a QGraphicsLayoutItem, which is neither MLayout,"
                  << "nor MWidget. Discarding";
        delete item;
        return 0;
    }
}

void GenericServiceSetupContext::addAccountSetting(AccountSetting *object)
{
    Q_D(GenericServiceSetupContext);

    d->accountSettings.append(object);
}

bool GenericServiceSetupContext::hasMandatorySettings() const
{
    QDomElement child =
        domDocument().documentElement().firstChildElement("ui");
    while (!child.isNull()) {
        if (child.firstChildElement().attribute("mandatory").compare("true") == 0) {
            return true;
        }
        child = child.nextSiblingElement();
    }
    return false;
}

} // namespace
