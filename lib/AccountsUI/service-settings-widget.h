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

#ifndef ACCOUNTSUI_SERVICESETTINGSWIDGET_H
#define ACCOUNTSUI_SERVICESETTINGSWIDGET_H

//M
#include <MContainer>

namespace AccountsUI {

    enum SettingsType {
        ShowMandatory = 0,
        ShowAll,
        ShowOnlySettings,
        ShowButtonAndSettings,
    };

class ServiceSettingsWidgetPrivate;
class AbstractServiceSetupContext;
/*!
 * @class ServiceSettingsWidget is the class to provide
 * service settings.
 *  It gives one switch to enable/disable settings and below are
 * all the settings.
 */
class ServiceSettingsWidget : public MWidget
{
Q_OBJECT;
public:
    /*!
     * Constuctor having following param
     * @param context is the Service context for which it has to create settings widget
     * @param parent is the parent widget
     * @param showOnlySettings is bool which will show only settings and not the toggle switch button when true
     * @param showAllSettings is bool which displays all settings when true and only mandatory settings when false
     */
    enum VisibleSettings {
        MandatorySettings = 1 << 0,
        NonMandatorySettings = 1 << 1,
        EnableButton = 1 << 2
    };

    ServiceSettingsWidget(AccountsUI::AbstractServiceSetupContext *context,
                          QGraphicsItem *parent = 0,
                          int settingsConf = MandatorySettings | EnableButton,
                          bool enabled = true
                          );
    ~ServiceSettingsWidget();
    void setServiceButtonEnable(bool enable);

    Q_SIGNALS:
        void serviceButtonEnabled(const QString &serviceType);

    private Q_SLOTS:
        void enabled(bool enabled);
        void openSettingsPage();

private:
    ServiceSettingsWidgetPrivate *d_ptr;
    Q_DISABLE_COPY(ServiceSettingsWidget)
    Q_DECLARE_PRIVATE(ServiceSettingsWidget)
};

} //namespace

#endif // ACCOUNTSUI_SERVICESETTINGSWIDGET_H
