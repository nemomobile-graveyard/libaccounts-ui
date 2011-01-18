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

#include "sort-service-model.h"
#include "service-model.h"

//Qt
#include <QDir>
#include <QSettings>
#include <QDebug>

bool FilterTypeServiceModel::filterAcceptsRow(int source_row,
                                              const QModelIndex &source_parent) const
{
    QModelIndex serviceIndex = sourceModel()->index(source_row, 0,
                                                    source_parent);
    const QVariant vServiceHelper = sourceModel()->data(serviceIndex,
                                                        ServiceModel::ServiceHelperColumn);
    ServiceHelper *serviceHelper = vServiceHelper.value<ServiceHelper *>();
    QString serviceType = serviceHelper->serviceType();
    if (serviceType == filterRegExp().pattern())
        return true;
    else
        return false;
}

SortServiceModel::SortServiceModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    /* testing purpose */
    QString testPath(getenv("SERVICES_PRIORITY"));
    if (!testPath.isEmpty()) {
        QDir testDir(testPath);
        QStringList testPriorityList = testDir.entryList(QDir::Files);
        foreach(QString filename, testPriorityList)
            createPriorities(testDir.filePath(filename));
    }
    else {
        QDir etc(QDir::rootPath() + QLatin1String("/etc/accounts-ui/service-sorting.d"));
        QDir home(QDir::homePath() + QLatin1String("/.config/accounts-ui/service-sorting.d"));

        QStringList etcPriorityList = etc.entryList(QDir::Files);
        foreach(QString filename, etcPriorityList)
            createPriorities(etc.filePath(filename));

        QStringList homePriorityList = home.entryList(QDir::Files);
        foreach(QString filename, homePriorityList)
            createPriorities(home.filePath(filename));
    }
}

void SortServiceModel::setEnabledServices(const Accounts::ServiceList &enabledServices)
{
    qDebug()<<Q_FUNC_INFO;
    foreach(Accounts::Service *service, enabledServices)
        enabledServicesName.append(service->name());
}

void SortServiceModel::setHiddenServices(const Accounts::ServiceList &hiddenServices)
{
    qDebug() << Q_FUNC_INFO;
    foreach (Accounts::Service *service, hiddenServices) {
        QString serviceName = service->name();
        if (!hiddenService.contains(serviceName))
            hiddenService.append(serviceName);
    }
}

void SortServiceModel::createPriorities(const QString &fileName)
{
    qDebug()<<__PRETTY_FUNCTION__;

    QSettings settings(fileName, QSettings::IniFormat);
    foreach(QString key, settings.allKeys()) {
        priorities.insert(key, settings.value(key, 0).toInt());
        //only the latest value will be used
        if (hiddenService.contains(key))
            hiddenService.removeOne(key);
        if (settings.value(key, 0).toInt() > 1000)
            hiddenService.append(key);
    }
}

bool SortServiceModel::lessThan(const QModelIndex &left,
                                const QModelIndex &right) const
{
    QString a = sourceModel()->data(left).toString();
    QString b = sourceModel()->data(right).toString();

    int aValue = priorities.value(a, 0);
    int bValue = priorities.value(b, 0);

    if (aValue == bValue)
        return (a.toLower() < b.toLower());

    return (aValue < bValue);
}

bool SortServiceModel::filterAcceptsRow(int source_row,
                                        const QModelIndex & source_parent) const
{
    QModelIndex source_index =
        sourceModel()->index(source_row, ServiceModel::ServiceNameColumn, source_parent);
    QString key =
        sourceModel()->data(source_index).toString();
    if (enabledServicesName.contains(key))
        return true;
    return !hiddenService.contains(key);
}
