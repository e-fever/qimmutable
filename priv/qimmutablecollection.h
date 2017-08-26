#ifndef QIMMUTABLECOLLECTION_H
#define QIMMUTABLECOLLECTION_H

#include <QList>
#include <QJSValue>

namespace QImmutable {

    // It is a wrapper of collection data in QList / QJSValue
    template <typename T>
    class Collection {

    public:

        Collection() {
        }

        Collection(const QList<T>& source) {
            m_source = source;
        }

        int size() const {
            return m_source.size();
        }

        T get(int index) const {
            return m_source[index];
        }

        const T& operator[](int index) const {
            return m_source[index];
        }

        bool isSharedWith(const Collection<T>& other) const {
            return m_source.isSharedWith(other.m_source);
        }

    private:
        QList<T> m_source;
    };

    template<>
    class Collection<QJSValue> {
    public:
        Collection() {

        }

        Collection(const QJSValue& source) {
            m_source = source;
        }

        int size() const {
            return m_source.property("length").toInt();
        }

        QJSValue get(int index) const {
            return m_source.property(index);
        }

        const QJSValue operator[](int index) const {
            return m_source.property(index);
        }

        bool isSharedWith(const Collection<QJSValue>& value) const {
            return m_source.strictlyEquals(value.m_source);
        }

    private:
        QJSValue m_source;
    };

}

#endif // QIMMUTABLECOLLECTION_H
