/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 * Contact: Lucian Horga <ext-lucian.horga@nokia.com>
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

#ifndef ACCOUNTLISTITEM_H
#define ACCOUNTLISTITEM_H

#include <MDetailedListItem>

class AccountListItem : public MDetailedListItem
{
    Q_OBJECT
    public:
        AccountListItem(MDetailedListItem::ItemStyle style, QGraphicsItem *parent = 0);
        virtual ~AccountListItem();
        void setSubtitleLabelEnabled(bool status);
};

#endif // ACCOUNTLISTITEM_H
