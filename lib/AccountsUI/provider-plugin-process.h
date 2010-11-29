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

#ifndef ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_H
#define ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_H

// AccountsUI
#include <AccountsUI/account-plugin-interface.h>
#include <AccountsUI/abstract-account-setup-context.h>
#include <AccountsUI/abstract-service-setup-context.h>

//Qt
#include <QObject>

namespace AccountsUI {

class ProviderPluginProcessPrivate;

/*!
 * @class ProviderPluginProcess
 * @headerfile AccountsUI/provider-plugin-process.h \
 * AccountsUI/ProviderPluginProcess
 * @brief Class to manage the account provider plugins as separate processes.
 *
 * @details The ProviderPluginProcess class takes care of handling the
 * communication protocol between the account plugin process and the accounts
 * UI.
 */
class ACCOUNTSUI_EXPORT ProviderPluginProcess : public QObject
{
    Q_OBJECT

public:
    /*!
     * Constructs the account provider plugin process.
     * @deprecated This constructor is deprecated and will be eventually
     * removed.
     */
    ProviderPluginProcess(AccountPluginInterface *plugin,
                          int &argc, char **argv);

    /*!
     * Constructs the account provider plugin process.
     */
    ProviderPluginProcess(int &argc, char **argv);

    virtual ~ProviderPluginProcess();

    /*!
     * Every account provider plugin must call this method, which takes
     * control of the process.
     * When this method return, the process can terminate.
     */
    int exec();

    /*!
     * Get the instance of the object.
     */
    static ProviderPluginProcess *instance();

    /*!
     * Get the context.
     */
    AbstractAccountSetupContext *setupContext() const;

    /*!
     * Gets the context object which handles account
     * creation/settings/deletion.
     *
     * @param account the account which is used
     * @param type is the requested page type which is needed.
     * @param parent The parent object for the context.
     */
    virtual AbstractAccountSetupContext *accountSetupContext(Accounts::Account *account,
                                                             SetupType type,
                                                             QObject *parent = 0);

    void emitQuitMainApp();

signals:
    void quitMainApp();

public Q_SLOTS:
    /*!
     * Clean termination of the plugin process.
     */
    void quit();

protected:
    /*!
     * Creates the main MApplicationPage for the account plugin.
     * The default implementation of this method checks what is the SetupType
     * of the context and returns AddAccountPage for CreateNew, and
     * AccountSettingsPage for EditExisting.
     *
     * You can override this method if the your account setup design is
     * different from what provided by these classes: with this method you can
     * get a handle to the page, subclass it, or provide a completely
     * different implementation of it.
     */
    virtual MApplicationPage *mainPage();

private:
    void init(int &argc, char **argv);
    void releaseStoreLock(AbstractServiceSetupContext *context = 0);
    ProviderPluginProcessPrivate *d_ptr;
    Q_DECLARE_PRIVATE(ProviderPluginProcess)
};

} // namespace

#endif // ACCOUNTSUI_PROVIDER_PLUGIN_PROCESS_H
