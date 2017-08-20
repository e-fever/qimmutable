#ifndef QIMMUTABLEQMLLISTMODEL_H
#define QIMMUTABLEQMLLISTMODEL_H

#include <QObject>
#include <QJSValue>
#include "qimmutablelistmodel.h"
#include "priv/qsfastdiffrunneralgo_p.h"

namespace QImmutable {

    class QmlListModel : public QImmutable::ListModel<QJSValue>
    {
        Q_OBJECT
        Q_PROPERTY(QString keyField READ keyField WRITE setKeyField NOTIFY keyFieldChanged)
    public:
        explicit QmlListModel(QObject *parent = nullptr);

        QString keyField() const;
        void setKeyField(const QString &keyField);

    signals:
        void keyFieldChanged();

    public slots:
    private:
        QString m_keyField;
    };

}

#endif // QIMMUTABLEQMLLISTMODEL_H
