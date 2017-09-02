#include <QJSValueIterator>
#include "qimmutableconvert.h"

template<>
QVariantMap QImmutable::convert(const QVariantMap& object) {
    return object;
}

template<>
QVariantMap QImmutable::convert(const QJSValue& object) {
    QVariantMap data;
    if (object.isObject()) {

        QJSValueIterator it(object);
        while (it.hasNext()) {
            it.next();
            if (it.value().isObject()) {
                // Prevent to convert an array / object to a QVariantList/QVariantMap respectively
                data[it.name()] = QVariant::fromValue<QJSValue>(it.value());
            } else {
                data[it.name()] = it.value().toVariant();
            }
        }
    }

    return data;
}
