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

#include "layouts.h"
#include <QDebug>
#include <QMetaObject>

#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MFlowLayoutPolicy>
#include <MLinearLayoutPolicy>

QList<QGraphicsLayoutItem*> ContextFreeLayout::children(const QDomElement &node,
                            GenericServiceSetupContext *context,
                            Parser *parser,
                            bool showAllSettings
                            )
{
    QList<QGraphicsLayoutItem*> list ;
    QDomElement child = node.firstChildElement();
    while (!child.isNull()) {
        if ((!child.attribute("mandatory").compare("true")) ||
           showAllSettings) {
            QGraphicsLayoutItem *item = parser->parse(child, context, showAllSettings);
            if (item) {
                list.append (item);
            } else {
                qDebug() << Q_FUNC_INFO << "did not generate anything for the layout, "
                         << "skipping that XML subtree";
            }
        }
        child = child.nextSiblingElement();
    }
    return list;
}

MLayout *Layout2Columns::layout(const QList<QGraphicsLayoutItem*> &child_list)
{
    int column = 0;
    int row = 0;

    MLayout *realFormLayout = new MLayout();
    MGridLayoutPolicy *formGridLayoutPolicy = new MGridLayoutPolicy(realFormLayout);

    foreach(QGraphicsLayoutItem *item, child_list) {
        formGridLayoutPolicy->addItem(item, row, column);
        column++;
        if (column > 1) {
            column = 0;
            row++;
        }
    }
    return realFormLayout;
}

MLayout *LayoutFlow::layout(const QList<QGraphicsLayoutItem*> &child_list)
{
    MLayout *flowLayout = new MLayout();
    MFlowLayoutPolicy *layoutPolicy = new MFlowLayoutPolicy(flowLayout);

    foreach (QGraphicsLayoutItem *item, child_list) {
        layoutPolicy->addItem(item);
    }
    return flowLayout;
}


MLayout *LayoutList::layout(const QList<QGraphicsLayoutItem*> &child_list)
{
    MLayout *linearLayout = new MLayout();
    MLinearLayoutPolicy *layoutPolicy = new MLinearLayoutPolicy(linearLayout,
                                                                    Qt::Vertical);
    foreach (QGraphicsLayoutItem *item, child_list) {
        layoutPolicy->addItem(item);
    }
    return linearLayout;
}
