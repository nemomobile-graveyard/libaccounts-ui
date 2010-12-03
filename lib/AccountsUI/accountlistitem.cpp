/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 * Contact: Lucian Horga <ext-lucian.horga@nokia.com>
 *
 * This software, including documentation, is protected by copyright controlled
 * by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating, any or all
 * of this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 */

#include "accountlistitem.h"

#include <MLabel>

AccountListItem::AccountListItem(MDetailedListItem::ItemStyle style, QGraphicsItem *parent)
        :MDetailedListItem(style, parent)
{}

AccountListItem::~AccountListItem()
{}

void AccountListItem::setSubtitleLabelEnabled(bool status)
{
    MLabel *label = subtitleLabelWidget();
    if (status)
        label->setStyleName("EnabledAccountState");
    else
        label->setStyleName("DisabledAccountState");
}
