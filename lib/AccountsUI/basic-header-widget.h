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

#ifndef ACCOUNTSUI_BASICHEADERWIDGET_H
#define ACCOUNTSUI_BASICHEADERWIDGET_H

#include <MWidgetController>
#include <QObject>

namespace AccountsUI {

enum BasicHeaderWidgetType {
    TitleAndSubtitle,
    IconWithTitleAndSubTitle
};

class BasicHeaderWidgetPrivate;

class BasicHeaderWidget : public MWidgetController
{
    Q_OBJECT

public:
    BasicHeaderWidget(BasicHeaderWidgetType type, QGraphicsItem *parent = 0);
    void setTitle(const QString &titleText);
    void setSubtitle(const QString &subtitleText);
    void setImage(const QString &imageId);

private:
    BasicHeaderWidgetPrivate *d_ptr;
    Q_DISABLE_COPY(BasicHeaderWidget)
    Q_DECLARE_PRIVATE(BasicHeaderWidget)
};

} // namespace

#endif // ACCOUNTSUI_BASICHEADERWIDGET_H

