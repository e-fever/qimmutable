#include "qimmutableqmllistmodel_p.h"

using namespace QImmutable;

QmlListModel::QmlListModel(QObject *parent) : VariantListModel(parent)
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

QJSValue QmlListModel::source() const
{
    return m_source;
}

void QmlListModel::setSource(const QJSValue &source)
{
    if (m_source.strictlyEquals(source)) {
        return;
    }

    QSFastDiffRunner<QJSValue> runner;
    QImmutable::FastDiffRunnerAlgo<QJSValue> algo;
    Item<QJSValue> wrapper;
    wrapper.keyField = m_keyField;
    algo.setWrapper(wrapper);

    QList<QSPatch> patches = algo.compare(m_source, source);
    runner.patch(this, patches);
    m_source = source;

    emit sourceChanged();
}

QStringList QmlListModel::fields() const
{
    return m_fields;
}

void QmlListModel::setFields(const QStringList &fields)
{
    m_fields = fields;
    emit fieldsChanged();
    setRoleNames(fields);
}
