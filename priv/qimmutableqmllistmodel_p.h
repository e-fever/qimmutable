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
    public:
        explicit QmlListModel(QObject *parent = nullptr);

        QString keyField() const;
        void setKeyField(const QString &keyField);

        QJSValue source() const;
        void setSource(const QJSValue &source);

    signals:
        void keyFieldChanged();
        void sourceChanged();

    public slots:
    private:
        QString m_keyField;
        QJSValue m_source;
    };

}

#endif // QIMMUTABLEQMLLISTMODEL_H
