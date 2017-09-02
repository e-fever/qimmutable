#pragma once
#include <QVariantMap>
#include <QJSValue>
#include <QMetaObject>
#include <QMetaProperty>

namespace QImmutable {

template <typename T>
QVariantMap convert(const T& object) {
    QVariantMap data;
    const QMetaObject meta = object.staticMetaObject;

    for (int i = 0 ; i < meta.propertyCount(); i++) {
        const QMetaProperty property = meta.property(i);
        const char* name = property.name();
        QVariant value = property.readOnGadget(&object);
        data[name] = value;
    }

    return data;
}

template<> QVariantMap convert(const QVariantMap& object);
template<> QVariantMap convert(const QJSValue& object);
}

