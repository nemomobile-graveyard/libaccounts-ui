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

#include "parser.h"

#include "ui-builder.h"
#include "settings-widgets.h"
#include "layouts.h"

#include "generic-service-setup-context.h"

#include <MLocale>
#include <MTheme>
#include <MLabel>
#include <QMetaObject>

namespace AccountsUI {
    class ParserPrivate
    {
    public:
        QHash<QString,const QMetaObject*> widgetMetaObjects; // a dictionary of recognized types
    };
};

Parser::Parser() : d_ptr(new ParserPrivate)
{
}


Parser::~Parser()
{
    Q_D(Parser);
    delete d;
};

void Parser::registerWidgetMeta(const QString &tag, const QMetaObject *object)
{
    Q_D(Parser);
    d->widgetMetaObjects.insert(tag, object);
}

// parse the branch under @node,
// and build the widgets, and possibly register with context for the Store()
// get initial value from @context.
QGraphicsLayoutItem *Parser::parse(const QDomElement &node,
              GenericServiceSetupContext *context,
              bool showAllSettings)
{
    Q_D(Parser);
    QString tag = node.tagName();
    if ((tag == "widget") || (tag == "layout"))  {
        QString type = node.attribute("type");

	UiBuilder *widget = 0;
        if (d->widgetMetaObjects.contains(type)) {
	    qDebug() << "metaobject invoking" << tag << type;
	    QObject *object = d->widgetMetaObjects[type]->newInstance (Q_ARG(QObject*, context));
	    if (!object)
		qDebug() << "newInstance() failed";
	    else {
		widget = qobject_cast<UiBuilder *>(object);
		if (!widget && object) {
		    qDebug() << "the sub-parser did not return UiBuilder";
		    delete object;
		}
	    }
	} else {
	    qDebug() << "unknown tag/type" << tag << type << "...skipping";
	}
	if (widget) {
	    qDebug() << "running UI()";
            return widget->ui(node, context, this, showAllSettings);
	}
    } else {
        qDebug() << "Unrecognized tag" << tag;
    }
    return 0;
}

// create a `Storable' UI, out of the XML/QDomDocument.
QGraphicsLayoutItem *Parser::parseDocument(const QDomDocument &doc,
                                           GenericServiceSetupContext *context,
                                           bool showAllSettings)
{
    qDebug() << Q_FUNC_INFO;

    QDomElement root = doc.documentElement();
    QDomElement node = root.firstChildElement("ui");
    if (node.isNull()) {
        qDebug() << "<ui> not found";
        return 0;
    }
    QDomElement childElement = node.firstChildElement();
    while (!childElement.isNull()) {
        if (childElement.tagName() == QLatin1String("trCatalog")) {
            MLocale locale;
            locale.installTrCatalog(node.text());
            MLocale::setDefault(locale);
        } else {
            return parse(childElement, context, showAllSettings);
        }
        childElement = childElement.nextSiblingElement();
    }
    return 0;
}
