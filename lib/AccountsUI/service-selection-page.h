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

#ifndef ACCOUNTSUI_SERVICESELECTIONPAGE_H
#define ACCOUNTSUI_SERVICESELECTIONPAGE_H

//libAccountsUI
#include <AccountsUI/abstract-account-setup-context.h>
#include <AccountsUI/abstract-service-setup-context.h>
#include <AccountsUI/account-sync-handler.h>
#include <AccountsUI/common.h>

//Meegotouch
#include <MApplicationPage>
#include <MWidget>

// Qt
#include <QModelIndex>

namespace AccountsUI {

class ServiceSelectionPagePrivate;

class ACCOUNTSUI_EXPORT ServiceSelectionPage: public MApplicationPage
{
    Q_OBJECT

public:
    explicit ServiceSelectionPage(AccountsUI::AbstractAccountSetupContext *context,
                                  QList<AccountsUI::AbstractServiceSetupContext*>
                                  &serviceContextList,
                                  QGraphicsItem *parent = 0);
    virtual ~ServiceSelectionPage();
    virtual void createContent();
    void setWidget(MWidget *widget);

private slots:
    void serviceSelected(QModelIndex);
    void onSyncStateChanged(const SyncState &state);
    void onAccountInstallButton();

Q_SIGNALS:
    void serviceEnabled(const QString &serviceName, bool enabled);

private:
    ServiceSelectionPagePrivate *d_ptr;
    Q_DISABLE_COPY(ServiceSelectionPage)
    Q_DECLARE_PRIVATE(ServiceSelectionPage)
};

} //namespace

#endif // ACCOUNTSUI_SERVICESELECTIONPAGE_H

