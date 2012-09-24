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
    QMap<ServiceModel::Columns, QVariant> headerData;
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
    init(account->services());
}

void ServiceModel::init(Accounts::ServiceList services)
{
    Q_D(ServiceModel);

    d->headerData.insert(ServiceHelperColumn, "serviceHelper");
    d->headerData.insert(ServiceNameColumn, "serviceName");
    d->headerData.insert(ColumnCount, "columncount");
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
    if (!index.isValid())
        return QVariant();
    ServiceHelper *serviceHelper = this->serviceHelper(index);
    if (!serviceHelper)
        return QVariant();
    if (role == Qt::DisplayRole) {
        if (index.column() == ServiceHelperColumn) {
            return QVariant::fromValue(serviceHelper);
        }
    if (index.column() == ServiceNameColumn)
        return serviceHelper->serviceName();

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

