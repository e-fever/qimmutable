#ifndef QIMMUTABLECOLLECTION_H
#define QIMMUTABLECOLLECTION_H

#include <QList>
#include <QJSValue>

namespace QImmutable {

    // It is a wrapper of collection data in QList / QJSValue
    template <typename T>
    class Collection {

    public:
        Collection(QList<T>& source) {
            m_source = source;
        }

        int size() const {
            return m_source.size();
        }

        T get(int index) const {
            return m_source[index];
        }

    private:
        QList<T> m_source;
    };

    template<>
    class Collection<QJSValue> {
    public:
        Collection(QJSValue source) {
            m_source = source;
        }

        int size() const {
            return m_source.property("length").toInt();
        }

        QJSValue get(int index) const {
            return m_source.property(index);
        }

    private:
        QJSValue m_source;
    };

}

#endif // QIMMUTABLECOLLECTION_H
