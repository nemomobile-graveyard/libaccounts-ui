/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2011 Nokia Corporation.
 *
 * Contact: Tomi Suviola <tomi.suviola@nokia.com>
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

#include "basic-item-widget.h"

#include <MImageWidget>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>

//Qt
#include <QDebug>

namespace AccountsUI {

class BasicItemWidgetPrivate: public QObject
{
public:
    BasicItemWidgetPrivate()
        :image(0),
        title(0),
        subtitle(0)
    {}

    ~BasicItemWidgetPrivate() {}

public:
    MImageWidget *image;
    MLabel *title;
    MLabel *subtitle;
    BasicItemWidget::BasicItemWidgetType type;
};

BasicItemWidget::BasicItemWidget(BasicItemWidgetType type, QGraphicsItem *parent)
        :MWidgetController(parent),
        d_ptr(new BasicItemWidgetPrivate())
{
    Q_D(BasicItemWidget);
    d->type = type;
}

void BasicItemWidget::createLayout()
{
    Q_D(BasicItemWidget);

    MLayout *mainLayout = new MLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    MLinearLayoutPolicy *mainLayoutPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Horizontal);

    MLayout *titleLayout = new MLayout();
    MLinearLayoutPolicy *titleLayoutPolicy = new MLinearLayoutPolicy(titleLayout, Qt::Vertical);

    d->title = new MLabel();
    d->title->setStyleName("CommonTitleInverted");
    d->title->setTextElide(true);
    d->subtitle = new MLabel();
    d->subtitle->setStyleName("CommonSubTitleInverted");

    d->subtitle->setPreferredHeight(QFontMetrics(d->subtitle->font()).height() * 5);
    d->subtitle->setWordWrap(true);
    d->subtitle->setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    d->subtitle->setTextElide(false);

    d->image = new MImageWidget();
    d->image->setStyleName("CommonMainIcon");

    switch(d->type) {
    case TitleAndSubtitle:
        titleLayoutPolicy->addItem(d->title, Qt::AlignLeft | Qt::AlignTop);
        titleLayoutPolicy->addItem(d->subtitle, Qt::AlignLeft | Qt::AlignTop);
        break;

    case IconWithTitle:
        titleLayoutPolicy->addItem(d->title, Qt::AlignLeft | Qt::AlignTop);
        mainLayoutPolicy->addItem(d->image, Qt::AlignCenter);
        break;

    case IconWithTitleAndSubTitle:
        titleLayoutPolicy->addItem(d->title, Qt::AlignLeft | Qt::AlignTop);
        titleLayoutPolicy->addItem(d->subtitle, Qt::AlignLeft | Qt::AlignTop);
        mainLayoutPolicy->addItem(d->image, Qt::AlignCenter);
        break;
    }

    mainLayoutPolicy->addItem(titleLayout, Qt::AlignLeft | Qt::AlignCenter);

    setLayout(mainLayout);
    updateGeometry();
}

void BasicItemWidget::setTitle(const QString &titleText)
{
    Q_D(BasicItemWidget);
    d->title->setText(titleText);
}

void BasicItemWidget::setSubtitle(const QString &subtitleText)
{
    Q_D(BasicItemWidget);
    d->subtitle->setText(subtitleText);
}

void BasicItemWidget::setImage(const QString &imageId)
{
    Q_D(BasicItemWidget);
    d->image->setImage(imageId);
}

} // namespace
