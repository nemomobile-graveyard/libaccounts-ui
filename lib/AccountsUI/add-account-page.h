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

#ifndef ACCOUNTSUI_ADDACCOUNTPAGE_H
#define ACCOUNTSUI_ADDACCOUNTPAGE_H

//libAccountsUI
#include <AccountsUI/abstract-account-setup-context.h>
#include <AccountsUI/account-sync-handler.h>
#include <AccountsUI/common.h>

//M
#include <QDeclarativeView>

namespace AccountsUI {

class AddAccountPagePrivate;
/*!
 * @class AddAccountPage
 * @headerfile AccountsUI/add-account-page.h AccountsUI/AddAccountPage
 * @brief Main UI element for creation of new accounts.
 *
 * @details The AddAccountPage is the page that appears when the user wants to
 * create a new account. It is created by the
 * ProviderPluginProcess::mainPage() method, right before starting the
 * execution of the MApplication.
 */
class ACCOUNTSUI_EXPORT AddAccountPage : public QGraphicsWidget
{
    Q_OBJECT

public:
    /*!
     * Class constructor.
     */
    explicit AddAccountPage(AccountsUI::AbstractAccountSetupContext *context);
    virtual ~AddAccountPage();

    /*!
     * Create the page contents.
     */
    virtual void createContent();

    /*!
     * Allows clients to hide services from service selection screen.
     */
    void setHiddenServices(const Accounts::ServiceList &hiddenServices);

    /*!
     * Returns the account sync handler
     */
    AccountSyncHandler *accountSyncHandler();

    /*!
     * Sets the label for the username.
     */
    void setUsernameDisplayString(const QString &displayString);

protected slots:
    void navigateToServiceSelectionPage();
    void onError(AccountsUI::ErrorCode, const QString &);

private slots:
    void clearServiceContextList();
    void onSyncStateChanged(const SyncState &state);

protected:
    virtual void openServiceSelectionPage(AccountsUI::AbstractAccountSetupContext *context,
                                     QList<AccountsUI::AbstractServiceSetupContext*>
                                     serviceContextList);

private:
    AddAccountPagePrivate *d_ptr;
    Q_DISABLE_COPY(AddAccountPage)
    Q_DECLARE_PRIVATE(AddAccountPage)
};

} // namespace

#endif // ACCOUNTSUI_ADDACCOUNTPAGE_H
