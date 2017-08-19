/* QImmutable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/e-fever/qimmutable
*/

#pragma once

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QPointer>
#include <QSharedPointer>
#include "qspatchable.h"
#include "qimmutablefunctions.h"

namespace QImmutable {
class VariantListModel : public QAbstractListModel, public QSPatchable
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit VariantListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    int count() const;

    QHash<int, QByteArray> roleNames() const;

    void setRoleNames(const QVariantMap& value);

    void setRoleNames(const QStringList& value);

    void setStorage(const QVariantList& value);

    QVariantList storage() const;

public slots:

    int indexOf(QString field,QVariant value) const;

    QVariantMap get(int i) const;

protected:
    virtual void insert(int index, const QVariantList &value);

    virtual void insert(int index, const QVariantMap& value);

    virtual void move(int from, int to, int count = 1);

    virtual void remove(int i , int count  = 1);

    virtual void set(int index,QVariantMap data);

    void setProperty(int index,QString property ,QVariant value);

    void append(const QVariantMap&value);

    void clear();

signals:
    void countChanged();

public slots:

private:

    QHash<int, QByteArray> m_roles;
    QHash<QString, int> m_rolesLookup;

    QVariantList m_storage;
};

}
