#include "immutablelistmodelfunctions.h"
#include "qsyncableqmlwrapper.h"

QSyncableQmlWrapper::QSyncableQmlWrapper(QObject *parent) : QObject(parent)
{

}

void QSyncableQmlWrapper::assign(QObject *dest, QJSValue source)
{
    return ImmutableListModelFunc::assign(dest, source);
}
