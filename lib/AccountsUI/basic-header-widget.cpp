/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2011 Nokia Corporation.
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

#include "basic-header-widget.h"

//M
#include <MLabel>
#include <MImageWidget>

//Qt
#include <QDebug>

namespace AccountsUI {

BasicHeaderWidget::BasicHeaderWidget(MBasicListItem::ItemStyle style,
                                     QGraphicsItem *parent)
    : MBasicListItem(style, parent)
{
    setStyleName("CommonPanelInverted");
    titleLabelWidget()->setTextElide(true);

    /* Any of the MBasicListItem's subwidget getters also create
     * the specific subwidget if it is not already created, so
     * calling them, only within context */
    switch (style) {
    case MBasicListItem::SingleTitle:
        titleLabelWidget()->setStyleName("CommonHeaderInverted");
        break;
    case MBasicListItem::TitleWithSubtitle:
        titleLabelWidget()->setStyleName("CommonTitleInverted");
        subtitleLabelWidget()->setStyleName("CommonSubTitleInverted");
        subtitleLabelWidget()->setTextElide(true);
        break;
    case MBasicListItem::IconWithTitle:
        imageWidget()->setStyleName("CommonMainIcon");
        titleLabelWidget()->setStyleName("CommonHeaderInverted");
        break;
    case MBasicListItem::IconWithTitleAndSubtitle:
        imageWidget()->setStyleName("CommonMainIcon");
        titleLabelWidget()->setStyleName("CommonTitleInverted");
        subtitleLabelWidget()->setStyleName("CommonSubTitleInverted");
        subtitleLabelWidget()->setTextElide(true);
        break;
    default:
        break;
    }
}

void BasicHeaderWidget::setImage(const QString &imageId)
{
    MBasicListItem::ItemStyle style = itemStyle();
    bool hasIconWidget =
        (style == MBasicListItem::IconWithTitle)
        || (style == IconWithTitleAndSubtitle);

    if (hasIconWidget)
        imageWidget()->setImage(imageId);
}

} // namespace
