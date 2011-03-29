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

#include <MImageWidget>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>

//Qt
#include <QDebug>

namespace AccountsUI {

class BasicHeaderWidgetPrivate: public QObject
{
public:
    BasicHeaderWidgetPrivate()
        :image(0),
        title(0),
        subtitle(0)
    {}

    ~BasicHeaderWidgetPrivate() {}

public:
    MImageWidget *image;
    MLabel *title;
    MLabel *subtitle;
};

BasicHeaderWidget::BasicHeaderWidget(QGraphicsItem *parent)
        :MWidget(parent),
        d_ptr(new BasicHeaderWidgetPrivate())
{
    Q_D(BasicHeaderWidget);

    MLayout *mainLayout = new MLayout();
    MLinearLayoutPolicy *mainLayoutPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Horizontal);
    mainLayoutPolicy->setStyleName("CommonPanelInverted");

    MLayout *titleLayout = new MLayout();
    MLinearLayoutPolicy *titleLayoutPolicy = new MLinearLayoutPolicy(titleLayout, Qt::Vertical);
    titleLayoutPolicy->setStyleName("CommonPanelInverted");

    d->image = new MImageWidget();
    d->image->setStyleName("CommonMainIcon");
    d->title = new MLabel();
    d->title->setStyleName("CommonTitleInverted");
    d->subtitle = new MLabel();
    d->subtitle->setStyleName("CommonSubTitleInverted");

    titleLayoutPolicy->addItem(d->title);
    titleLayoutPolicy->addItem(d->subtitle);

    mainLayoutPolicy->addItem(d->image);
    mainLayoutPolicy->addItem(titleLayout);

    setLayout(mainLayout);
}

void BasicHeaderWidget::setTitle(const QString &titleText)
{
    Q_D(BasicHeaderWidget);
    d->title->setText(titleText);
}

void BasicHeaderWidget::setSubtitle(const QString &subtitleText)
{
    Q_D(BasicHeaderWidget);
    d->subtitle->setText(subtitleText);
}

void BasicHeaderWidget::setImage(const QString &imageId)
{
    Q_D(BasicHeaderWidget);
    d->image->setImage(imageId);
}

} // namespace
