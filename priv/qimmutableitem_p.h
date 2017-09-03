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

};

template<>
class Item<QVariantMap> {
public:
    inline bool isShared(const QVariantMap& v1, const QVariantMap& v2) const {
        return v1.isSharedWith(v2);
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
