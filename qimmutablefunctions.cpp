#include <QMetaProperty>
#include <QtQml>
#include "qimmutablefunctions.h"

void QImmutable::assign(QVariantMap &dest, const QObject *source)
{
    const QMetaObject* meta = source->metaObject();

    for (int i = 0 ; i < meta->propertyCount(); i++) {
        const QMetaProperty property = meta->property(i);
        QString p = property.name();

        QVariant value = source->property(property.name());

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>()); // nested properties is not supported yet
            value = map;
        }

        dest[p] = value;
    }

}

void QImmutable::assign(QObject *dest, const QVariantMap & source)
{
    const QMetaObject* meta = dest->metaObject();

    QMap<QString,QVariant>::const_iterator iter = source.begin();
    while (iter != source.end()) {
        QByteArray key = iter.key().toLocal8Bit();

        int index = meta->indexOfProperty(key.constData());
        if (index < 0) {
            qWarning() << QString("ImmutableListModelFunc::assign:assign a non-existed property: %1").arg(iter.key());
            iter++;
            continue;
        }

        QVariant orig = dest->property(key.constData());
        QVariant value = source[iter.key()];

        if (orig.canConvert<QObject*>()) {
            if (value.type() != QVariant::Map) {
                qWarning() << QString("ImmutableListModelFunc::assign:expect a QVariantMap property but it is not: %1");
            } else {
                assign(orig.value<QObject*>(), value.toMap());
            }

        } else if (orig != value) {
            dest->setProperty(key.constData(), value);
        }

        iter++;
    }
}

void QImmutable::assign(QObject *dest, const QJSValue &source)
{
    if (dest == 0) {
        return;
    }

    const QMetaObject* meta = dest->metaObject();
    QJSValueIterator iter(source);

    while (iter.hasNext()) {
        iter.next();
        QByteArray key = iter.name().toLocal8Bit();
        int index = meta->indexOfProperty(key.constData());
        if (index < 0) {
            qWarning() << QString("ImmutableListModelFunc::assign:assign a non-existed property: %1").arg(iter.name());
            continue;
        }

        QVariant orig = dest->property(key.constData());

        if (orig.canConvert<QObject*>()) {
            if (!iter.value().isObject()) {
                qWarning() << QString("ImmutableListModelFunc::assign:expect a object property but it is not: %1");
            } else {
                assign(orig.value<QObject*>(), iter.value());
            }
            continue;
        }

        QVariant value = iter.value().toVariant();
        if (orig != value) {
            dest->setProperty(key.constData(), value);
        }
    }

}

static QVariant _get(const QVariantMap& object, const QStringList &path, const QVariant& defaultValue) ;

static QVariant _get(const QObject* object, const QStringList &path, const QVariant& defaultValue) {

    QString key = path[0];

    const QMetaObject* meta = object->metaObject();

    if (meta->indexOfProperty(key.toUtf8().constData()) < 0) {
        return defaultValue;
    }

    QVariant value = object->property(key.toUtf8().constData());

    if (path.size() == 1) {
        return value;
    }

    QStringList nextPath = path;
    nextPath.removeFirst();

    if (value.canConvert<QObject*>()) {
        return _get(qvariant_cast<QObject*>(value), nextPath, defaultValue);
    } else if (value.type() == QVariant::Map) {
        return _get(value.toMap(), nextPath, defaultValue);
    } else {
        return defaultValue;
    }
}

static QVariant _get(const QVariantMap& object, const QStringList &path, const QVariant& defaultValue) {

    QString key = path[0];

    if (!object.contains(key)) {
        return defaultValue;
    }

    QVariant value = object[key];

    if (path.size() == 1) {
        return value;
    }

    QStringList nextPath = path;
    nextPath.removeFirst();

    if (value.canConvert<QObject*>()) {
        return _get(qvariant_cast<QObject*>(value), nextPath, defaultValue);
    } else if (value.type() == QVariant::Map) {
        return _get(value.toMap(), nextPath, defaultValue);
    } else {
        return defaultValue;
    }
}

QVariant QImmutable::get(const QObject *object, const QString &path, const QVariant& defaultValue)
{
    return get(object, path.split("."), defaultValue);
}

QVariant QImmutable::get(const QObject *object, const QStringList &path, const QVariant& defaultValue)
{
    return _get(object, path, defaultValue);
}

QVariant QImmutable::get(const QVariantMap &source, const QString &path, const QVariant &defaultValue)
{
    return get(source, path.split("."), defaultValue);
}

QVariant QImmutable::get(const QVariantMap &source, const QStringList &path, const QVariant &defaultValue)
{
    return _get(source, path, defaultValue);
}

void QImmutable::set(QVariantMap &data, const QString &path, const QVariant &value)
{
    return set(data, path.split("."), value);
}

void QImmutable::set(QVariantMap &data, const QStringList &path, const QVariant &value)
{
    QString key = path[0];

    if (path.size() == 1) {
        data[key] = value;
    } else {
        if (!data.contains(key) || !data[key].canConvert<QVariantMap>()) {
            data[key] = QVariantMap();
        }
        QStringList nextPath = path;
        nextPath.removeFirst();
        QVariantMap map = data[key].toMap();
        set(map, nextPath, value);
        data[key] = map;
    }
}

QVariantMap QImmutable::pick(QObject *object, const QStringList &paths)
{
    QVariantMap data;
    foreach (QString path, paths) {
        QVariant value = get(object, path);
        if (value.isNull()) {
            continue;
        }

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>());
            value = map;
        }

        set(data, path, value);
    }
    return data;
}


QVariantMap QImmutable::pick(QVariantMap source, const QStringList &paths)
{
    QVariantMap data;
    foreach (QString path, paths) {
        QVariant value = get(source, path);
        if (value.isNull()) {
            continue;
        }

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>());
            value = map;
        }

        set(data, path, value);
    }
    return data;
}

QVariantMap QImmutable::pick(QVariantMap source, const QVariantMap &paths)
{
    return pick(source, paths.keys());
}

QVariantMap QImmutable::omit(const QVariantMap &source, const QVariantMap &properties)
{

    QMap<QString,QVariant>::const_iterator iter = source.begin();
    QVariantMap result;

    while (iter != source.end()) {

        if (properties.contains(iter.key())) {
            iter++;
            continue;
        }

        QVariant value = source[iter.key()];

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>());
            value = map;
        }

        result[iter.key()] = value;
        iter++;
    }

    return result;
}


QVariantMap QImmutable::diff(const QVariantMap &v1, const QVariantMap &v2)
{
    auto prev = v1;
    auto current = v2;

    QVariantMap res;
    QMap<QString, QVariant>::const_iterator iter = current.begin();

    while (iter != current.end()) {
        QString key = iter.key();
        if (!prev.contains(key) ||
             prev[key] != iter.value()) {
            res[key] = iter.value();
        }
        iter++;
    }

    return res;
}

bool QImmutable::fastCompare(QVariant v1, QVariant v2)
{
    if (v1.userType() != v2.userType()) {
        return false;
    }

    return v1 == v2;
}
