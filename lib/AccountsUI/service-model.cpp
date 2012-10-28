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

#include "service-model.h"
#include <AccountsUI/service-helper.h>

//project library
#include <AccountsUI/AccountsManagerSingleton>
#include <QDebug>
#include <MButton>
#include <MWidget>

class ServiceModelPrivate
{
public:
    ~ServiceModelPrivate()
    {
        qDeleteAll(serviceList);
    }
    QList<ServiceHelper *> serviceList;
    QHash<int, QByteArray> headerData;
    Accounts::Account *account;
};

ServiceModel::ServiceModel(QObject *parent)
    : QAbstractTableModel(parent)
    , d_ptr(new ServiceModelPrivate())
{
    init(AccountsManager::instance()->serviceList());
}

ServiceModel::ServiceModel(Accounts::Account *account, QObject *parent)
    : QAbstractTableModel(parent)
    , d_ptr(new ServiceModelPrivate())
{
    Q_D(ServiceModel);
    d->account = account;
    init(account->services());
}

void ServiceModel::init(Accounts::ServiceList services)
{
    Q_D(ServiceModel);

    d->headerData.insert(ServiceHelperRole, "serviceHelper");
    d->headerData.insert(ServiceNameRole, "serviceName");
    d->headerData.insert(ServiceDescRole, "serviceDesc");
    d->headerData.insert(ServiceStatusRole, "serviceStatus");
    d->headerData.insert(ColumnCountRole, "columncount");
    setRoleNames(d->headerData);
    for (int i = 0; i < services.size(); i++)
    {
        QDomDocument domDocument = services[i].domDocument();
        d->serviceList << new ServiceHelper(services[i]);
    }
}

QVariant ServiceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const ServiceModel);
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }
    Columns column = static_cast<Columns>(section);
    if (role == Qt::DisplayRole) {
        if (section < d->headerData.size()) {
            return d->headerData.value(column);
        }
    }
    return QVariant();
}

ServiceModel::~ServiceModel()
{
    Q_D(ServiceModel);
    delete d;
}

ServiceHelper *
ServiceModel::serviceHelper(const QModelIndex& index) const
{
    Q_D(const ServiceModel);
    Q_ASSERT(d->serviceList.count() > index.row());
    if (d->serviceList.count() < index.row()) {
        return 0;
    }
    return d->serviceList.at(index.row());
}

const QDomDocument ServiceModel::domDocument(const QModelIndex& currentIndex) const
{
    ServiceHelper *serviceHelper;
    serviceHelper = this->serviceHelper(currentIndex);
    if (!serviceHelper)
        return QDomDocument();
    return serviceHelper->domDocument();
}


bool ServiceModel::isEmpty() const
{
    return (0 >= rowCount());
}

QModelIndex ServiceModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || column < 0 || !hasIndex(row, column, parent))
        return QModelIndex();
    return createIndex(row, column);
}

int ServiceModel::rowCount(const QModelIndex& parent) const
{
    Q_D(const ServiceModel);
    if (parent.isValid())
        return 0;
    return d->serviceList.count();
}

int ServiceModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant ServiceModel::data(const QModelIndex& index, int role) const
{
    Q_D(const ServiceModel);
    if (!index.isValid())
        return QVariant();
    ServiceHelper *serviceHelper = this->serviceHelper(index);
    if (!serviceHelper)
        return QVariant();


    if (role == ServiceHelperRole ||
            (role == Qt::DisplayRole && index.column() == ServiceHelperColumn))
        return QVariant::fromValue(serviceHelper);

    if (role == ServiceNameRole ||
            (role == Qt::DisplayRole && index.column() == ServiceNameColumn))
        return serviceHelper->serviceName();

    if (role == ServiceDescRole ||
            (role == Qt::DisplayRole && index.column() == ServiceDescColumn))
        return serviceHelper->description();

    if (role == ServiceStatusRole ||
            (role == Qt::DisplayRole && index.column() == ServiceStatusColumn)) {
        if (d->account) {
            Accounts::Service service = serviceHelper->service();
            d->account->selectService(service);
            return d->account->enabled();
        } else
            return QVariant::fromValue(true); //keeping them enabled while acount creation
    }

    return QVariant();
}

QList<AbstractServiceSetupContext*> ServiceModel::createServiceContexts(
    QAbstractItemModel *model,
    AbstractAccountSetupContext *context,
    QObject *parent)
{
    QList<AbstractServiceSetupContext*>  serviceContextlist;
    for (int i = 0; i<  model->rowCount(); i++) {
        QModelIndex index = model->index(i, 0);
        const QVariant vServiceHelper = index.data(ServiceModel::ServiceHelperColumn);
        ServiceHelper *serviceHelper = vServiceHelper.value<ServiceHelper *>();
        AbstractServiceSetupContext *serviceContext =
            serviceHelper->serviceSetupContext(context, parent);
        serviceContextlist.append(serviceContext);
    }
    return serviceContextlist;
}

