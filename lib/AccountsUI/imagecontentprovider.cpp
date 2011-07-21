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

#include <QDebug>
#include <QtSparql>
#include <TrackerLiveQuery>
#include <gallerymodel.h>

#include "imagecontentprovider.h"

static const QString query = "SELECT "
                                 "nie:url(?urn) AS ?url "
                                 "nfo:fileName(?urn) AS ?filename "
                                 "nie:contentCreated(?urn) AS ?created "
                                 "nie:mimeType(?urn) AS ?mimetype "
                                 "( EXISTS { "
                                 "    ?urn nao:hasTag nao:predefined-tag-favorite . "
                                 "} ) AS ?favorite "
                                 "nfo:duration(?urn) AS ?duration "
                                 "?urn tracker:id(?urn) AS ?trackerid "
                                 "WHERE { "
                                 "?urn rdf:type nmm:Photo . "
                                 "?urn tracker:available \"true\"^^xsd:boolean . "
                                 "} "
                                 "ORDER BY "
                                 "DESC (?created) "
                                 "DESC (?filename)";

ImageContentProvider::ImageContentProvider(GalleryModel& galleryModel, QObject* parent) :
        GalleryContentProvider(parent),
        liveQuery(new TrackerLiveQuery(query, 8, *galleryModel.sparqlConnection())),
        queryRunning(false)
{
    qDebug() << Q_FUNC_INFO;
    liveQuery->setUpdatesEnabled(true);
}

ImageContentProvider::~ImageContentProvider()
{
    qDebug() << Q_FUNC_INFO;
    delete liveQuery;
}

QAbstractItemModel* ImageContentProvider::model() const
{
    return liveQuery ? liveQuery->model() : 0;
}

void ImageContentProvider::queryContent(int limit)
{
    qDebug() << Q_FUNC_INFO << limit;
    Q_UNUSED(limit);

    queryRunning = true;

    TrackerPartialUpdater updater(query);
    updater.watchClass("nmm:Photo", QStringList(),
                       "tracker:id(?urn) in %LIST", TrackerPartialUpdater::Subject, 7);
    liveQuery->addUpdater(updater);
    liveQuery->setIdentityColumns(QList<int>() << 7);

    // Collate by creation time first, file name then.
    liveQuery->setCollationColumns(QList<TrackerLiveQuery::CollationColumn>()
                                    << TrackerLiveQuery::CollationColumn(GalleryContentProvider::CreatedColumn,
                                                                         QVariant::DateTime,
                                                                         Qt::DescendingOrder)
                                    << TrackerLiveQuery::CollationColumn(GalleryContentProvider::FileNameColumn,
                                                                         QVariant::String,
                                                                         Qt::DescendingOrder));
    connect(liveQuery,
                    SIGNAL(initialQueryFinished()),
                    this, SLOT(onInitialQueryFinished()),
                    Qt::UniqueConnection);
    liveQuery->start();
}

bool ImageContentProvider::isLoadingContent() const
{
    qDebug() << Q_FUNC_INFO;
    return queryRunning;
}

void ImageContentProvider::onInitialQueryFinished()
{
    qDebug() << Q_FUNC_INFO;
    queryRunning = false;
}

