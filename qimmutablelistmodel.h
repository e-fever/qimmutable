#pragma once
#include <qimmutablevariantlistmodel.h>
#include <qsfastdiffrunner.h>
#include <functional>
#include <qimmutableconvert.h>


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

            FastDiffRunner<T> runner;
            if (m_customConvertor != nullptr) {
                runner.setCustomConvertor(m_customConvertor);
            }
            QList<QSPatch> patches = runner.compare(m_source, source);
            m_source = source;
            runner.patch(this, patches);
        }

        void setCustomConvertor(const std::function<QVariantMap (T, int)> &customConvertor) {
            m_customConvertor = customConvertor;
        }

    private:

        QList<T> m_source;
        std::function<QVariantMap(T, int)> m_customConvertor;

    };

}
