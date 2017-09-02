#pragma once
#include <QVariantMap>
#include <QMetaMethod>
#include <QJSValue>
#include <QJSValueIterator>
#include "qimmutablefunctions.h"

namespace QImmutable {

/// It is a wrapper of an Immutable type
template <typename T>
class Item {

public:
    inline bool isShared(const T& v1, const T& v2) const {
        return QImmutable::isShared(v1, v2);
    }

    inline QVariantMap convert(const T& object) {
        // Default convertor function
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

    inline bool hasKey() {
        const QMetaObject meta = T::staticMetaObject;
        return meta.indexOfMethod("key()") >= 0;
    }

    QString key(const T& value) {
        QString ret;
        const QMetaObject meta = T::staticMetaObject;
        int index = meta.indexOfMethod("key()");
        if (index < 0) {
            return ret;
        }

        QMetaMethod method = meta.method(index);

        if (method.returnType() == QVariant::Int) {
            int iRet;
            method.invokeOnGadget((void*) &value, Q_RETURN_ARG(int, iRet));
            ret = QString::number(iRet);

        } else if (method.returnType() == QVariant::String) {
            method.invokeOnGadget((void*) &value, Q_RETURN_ARG(QString, ret));
        } else {
            qWarning() << "QSyncable::Invalid key type";
        }

        return ret;
    }

    QVariantMap diff(const T& v1, const T& v2) {
        auto prev = convert(v1);
        auto current = convert(v2);

        return QImmutable::diff(prev, current);
    }

    QVariantMap fastDiff(const T& v1, const T& v2) {
        if (isShared(v1,v2)) {
            return QVariantMap();
        }
        return diff(v1, v2);
    }
};

template<>
class Item<QVariantMap> {
public:
    inline bool isShared(const QVariantMap& v1, const QVariantMap& v2) const {
        return v1.isSharedWith(v2);
    }

    inline QVariantMap convert(const QVariantMap& object) {
        return object;
    }

    QVariantMap diff(const QVariantMap& v1, const QVariantMap& v2) {
        return QImmutable::diff(v1, v2);
    }

    QVariantMap fastDiff(const QVariantMap& v1, const QVariantMap& v2) {
        if (isShared(v1,v2)) {
            return QVariantMap();
        }
        return diff(v1, v2);
    }

    bool hasKey() {
        return !keyField.isNull();
    }

    QString key(const QVariantMap& object) {
        if (keyField.isNull()) {
            return QString();
        }

        QString res;
        QVariant value = object[keyField];
        if (value.type() == QVariant::Int) {
            res = QString::number(value.toInt());
        } else if (value.type() == QVariant::String) {
            res = value.toString();
        }
        return res;
    }

    QString keyField;

};


template<>
class Item<QJSValue> {
public:
    inline bool isShared(const QJSValue& v1, const QJSValue& v2) const {        
        if (v1.isNull() || v1.isUndefined() || v2.isNull() || v2.isUndefined()) {
            // Null is not considered as shared
            return false;
        }
        return v1.strictlyEquals(v2);
    }

    inline QVariantMap convert(const QJSValue& object) {
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

    QVariantMap diff(const QJSValue& v1, const QJSValue& v2) {
        return QImmutable::diff(convert(v1), convert(v2));
    }

    QVariantMap fastDiff(const QJSValue& v1, const QJSValue& v2) {
        if (!isShared(v1, v2)) {
            return QVariantMap();
        }
        auto prev = convert(v1);
        auto current = convert(v2);

        return QImmutable::diff(prev, current);
    }

    bool hasKey() {
        return !keyField.isNull();
    }

    QString key(const QJSValue& object) {
        if (keyField.isNull()) {
            return QString();
        }

        QString res;
        QJSValue value = object.property(keyField);
        if (value.isNumber()) {
            res = QString::number(value.toInt());
        } else if (value.isString()) {
            res = value.toString();
        }
        return res;
    }

    QString keyField;

};

}
