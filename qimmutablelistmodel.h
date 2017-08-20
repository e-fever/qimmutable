#pragma once
#include <qimmutablevariantlistmodel.h>
#include <qsfastdiffrunner.h>

namespace QImmutable {

    template <typename T>
    class ListModel: public VariantListModel {
    public:
        ListModel(QObject* parent = 0) : VariantListModel(parent) {
        }

        QList<T> source() const
        {
            return m_source;
        }

        void setSource(const QList<T> &source)
        {

            if (m_source.isSharedWith(source)) {
                return;
            }

            QSFastDiffRunner<T> runner;
            QList<QSPatch> patches = runner.compare(m_source, source);
            m_source = source;
            runner.patch(this, patches);
        }

    private:

        QList<T> m_source;

    };
}
