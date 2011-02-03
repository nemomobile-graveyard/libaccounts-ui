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

#ifndef ACCOUNTSUI_SETTINGSPAGE_H
#define ACCOUNTSUI_SETTINGSPAGE_H

//M
#include <MApplicationPage>
#include <MButton>

namespace AccountsUI {
class SettingsPagePrivate;
class AbstractServiceSetupContext;

class SettingsPage: public MApplicationPage
{
    Q_OBJECT

public:
    SettingsPage(AccountsUI::AbstractServiceSetupContext *context,
                 MButtonModel *model,
                 QGraphicsItem *parent = 0);
    virtual ~SettingsPage();
    virtual void createContent();

private slots:
    void changeServiceStatus();

private:
    SettingsPagePrivate *d_ptr;
    Q_DISABLE_COPY(SettingsPage)
    Q_DECLARE_PRIVATE(SettingsPage)
};

} //namespace

#endif // ACCOUNTSUI_SETTINGSPAGE_H
