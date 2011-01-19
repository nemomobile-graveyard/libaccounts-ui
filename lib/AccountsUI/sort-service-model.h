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

#ifndef ACCOUNTSUI_SORT_SERVICE_MODEL_H
#define ACCOUNTSUI_SORT_SERVICE_MODEL_H

//libAccountsUI
#include <AccountsUI/common.h>

//Accounts
#include <Accounts/Account>

//Qt
#include <QSortFilterProxyModel>
#include <QStringList>

class ACCOUNTSUI_EXPORT FilterTypeServiceModel : public QSortFilterProxyModel
{
public:
    FilterTypeServiceModel(QObject *parent = 0) : QSortFilterProxyModel(parent)
    {}
    ~FilterTypeServiceModel()
    {}
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

/*!
 * @class ServiceModel
 * @headerfile AccountsUI/service-model.h AccountsUI/ServiceModel
 *
 * Sorts/filters the ServiceModel items according to the configuration files
 * stored in the /etc/accounts-ui/service-sorting.d/ and
 * ~/.config/accounts-ui/service-sorting.d/ directories.
 */
class ACCOUNTSUI_EXPORT SortServiceModel : public QSortFilterProxyModel
{
public:
    SortServiceModel(QObject *parent = 0);
    virtual ~SortServiceModel() {};
    void setEnabledServices(const Accounts::ServiceList &enabledServices);
    void setHiddenServices(const Accounts::ServiceList &hiddenServices);

protected:
    virtual bool lessThan(const QModelIndex &left,
                          const QModelIndex &right) const;
    virtual bool filterAcceptsRow(int source_row,
                                  const QModelIndex &source_parent) const;
    void createPriorities(const QString &fileName);

private:
    QHash<QString, int> priorities;
    QStringList hiddenService;
    QStringList enabledServicesName;
};

#endif // ACCOUNTSUI_SORT_SERVICE_MODEL_H
