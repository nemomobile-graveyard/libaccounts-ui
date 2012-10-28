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

#ifndef ACCOUNTSUI_SERVICEMODEL_H
#define ACCOUNTSUI_SERVICEMODEL_H

//libAccountsUI
#include <AccountsUI/service-helper.h>
#include <AccountsUI/abstract-account-setup-context.h>

//Qt
#include <QAbstractTableModel>
#include <QDomDocument>
#include <QModelIndex>

using namespace AccountsUI;
class ServiceModelPrivate;

/*!
 * @class ServiceModel
 * @headerfile AccountsUI/service-model.h AccountsUI/ServiceModel
 *
 * The ServiceModel is a model which provide an interface to all available
 * services
 * It looks into a directory where are the xml files for every kind of account profiles.
 */

class ACCOUNTSUI_EXPORT ServiceModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /*!
     * This enumerator holds the roles used/provided data roles of the model
     */
    enum Columns{
        ServiceHelperColumn,
        ServiceNameColumn,
        ServiceDescColumn,
        ServiceStatusColumn,
        ColumnCount
    };

    enum Roles{
        ServiceHelperRole,
        ServiceNameRole,
        ServiceDescRole,
        ServiceStatusRole,
        ColumnCountRole
    };

#ifndef ACCOUNTSUI_DISABLE_DEPRECATED
    /*!
     * @deprecated
     * Constructor creates an Model
     * \param parent The parent Object.
     */
    ServiceModel(QObject *parent);
#endif

    /*!
     *  Constructor creates an Model
     * \param account The Account object.
     * \param parent The parent Object.
     */
    ServiceModel(Accounts::Account *account, QObject *parent = 0);

    /*!
     * The Destructor.
     */
    ~ServiceModel();

    ServiceHelper *serviceHelper(const QModelIndex& index) const;

    /*!
     * To get the domDocument which is a const shallow copy for this index
     * \param an index which is fresh from model
     */
    const QDomDocument domDocument(const QModelIndex& index) const;

    /*!
     *  Derived from QAbstractListModel - delivers the number of rows in the model.
     * \return The number of valid Accounts given by the AccountManager or 0 if index is a valid
     * QModelIndex
     * \param index Will lead to returning 0 if it is a valid QModelIndex. Since we have a simple
     *  Listmodel and no treemodel there should be no need to set this.
     */
    int rowCount( const QModelIndex & index = QModelIndex() ) const;

    /*!
     *  Derived from QAbstractListModel - returns the number of columns in the model.
     * \return The number of columns in the model or 0 if <i> index </i> is a valid QModelIndex
     * \param index Will lead to returning 0 if it is a valid QModelIndex. Since we have a simple
     *  Listmodel and no treemodel there should be no need to set this.
     */
    int columnCount( const QModelIndex & index ) const;

    /*!
     *  Provides a getter for the model-data depending on the index and the role provided as
     *  Parameter.
     * \return ModelData depending on given <i>index</i> and <i>role</i> capsuled in a QVariant.
     * \param index A QModelindex that is related to the requested data
     * \param role The role the data is requested for \ref Qt::ItemDataRole.
     */
    QVariant data( const QModelIndex &index, int role ) const;

    /*!
     *  Derived from QAbstractTableModel - Provides a way to get a QModelIndex for a certain
     *  <i>row</i>, <i>column</i> and <i>parent</i> combination.
     * \return A QModelIndex related to the given combination or a QModelIndex() if given row is
     *   invalid.
     * \param row The row of the requested index.
     * \param column The column for the requested index;
     * \param parent The QModelIndex of the parent of the requested index' parent
     */
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;

    /*!
     *  Can be used to clarify whether the model is empty or not.
     * \return True if there is no data in the model or false otherwise.
     */
    bool isEmpty() const;

    /*!
     *  Reads all available Service Profiles
     */
    void refresh();
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /*!
     * Returns the service Contexts of all the services for an account.
     */
    static QList<AbstractServiceSetupContext*> createServiceContexts(
        QAbstractItemModel *model,
        AbstractAccountSetupContext *context,
        QObject *parent = 0);


private:
    ServiceModelPrivate* d_ptr;
    Q_DISABLE_COPY(ServiceModel)
    Q_DECLARE_PRIVATE(ServiceModel);
    void init(Accounts::ServiceList services);
};

Q_DECLARE_METATYPE(AccountsUI::ServiceHelper *)

#endif // ACCOUNTSUI_SERVICEMODEL_H
