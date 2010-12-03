/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 *
 * This software, including documentation, is protected by copyright controlled
 * by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating, any or all
 * of this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
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
