#include "qimmutablefunctions.h"
#include "qsyncableqmlwrapper.h"

QSyncableQmlWrapper::QSyncableQmlWrapper(QObject *parent) : QObject(parent)
{

}

void QSyncableQmlWrapper::assign(QObject *dest, QJSValue source)
{
    return QImmutable::assign(dest, source);
}
