#ifndef QIMMUTABLEQMLLISTMODEL_H
#define QIMMUTABLEQMLLISTMODEL_H

#include <QObject>
#include <QJSValue>
#include "qimmutablelistmodel.h"
#include "priv/qsfastdiffrunneralgo_p.h"

namespace QImmutable {

    class QmlListModel : public VariantListModel
    {
        Q_OBJECT
        Q_PROPERTY(QString keyField READ keyField WRITE setKeyField NOTIFY keyFieldChanged)
        Q_PROPERTY(QJSValue source READ source WRITE setSource NOTIFY sourceChanged)
        Q_PROPERTY(QStringList fields READ fields WRITE setFields NOTIFY fieldsChanged)
    public:
        explicit QmlListModel(QObject *parent = nullptr);

        QString keyField() const;
        void setKeyField(const QString &keyField);

        QJSValue source() const;
        void setSource(const QJSValue &source);

        QStringList fields() const;
        void setFields(const QStringList &fields);

    signals:
        void keyFieldChanged();
        void sourceChanged();
        void fieldsChanged();

    public slots:
    private:
        QString m_keyField;
        QJSValue m_source;
        QStringList m_fields;

    };

}

#endif // QIMMUTABLEQMLLISTMODEL_H
