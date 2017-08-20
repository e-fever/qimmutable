#include "qimmutableqmllistmodel_p.h"

using namespace QImmutable;

QmlListModel::QmlListModel(QObject *parent) : QImmutable::ListModel<QJSValue>(parent)
{

}

QString QmlListModel::keyField() const
{
    return m_keyField;
}

void QmlListModel::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
    emit keyFieldChanged();
}
