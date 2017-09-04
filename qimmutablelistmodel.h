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
            m_processing = false;
        }

        QList<T> source() const
        {
            return m_source;
        }

        void setSource(const QList<T> &source)
        {
            if (m_processing) {
                m_queue.enqueue(source);
                return;
            }

            m_processing = true;

            auto process = [&](const QList<T> & source) {
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
            };

            process(source);

            while (m_queue.size() > 0) {
                QList<T> input = m_queue.dequeue();
                process(input);
            }
            m_processing = false;
        }

        void setCustomConvertor(const std::function<QVariantMap (T, int)> &customConvertor) {
            m_customConvertor = customConvertor;
        }

    private:

        QList<T> m_source;
        std::function<QVariantMap(T, int)> m_customConvertor;
        bool m_processing;
        QQueue<QList<T>> m_queue;


    };

}
