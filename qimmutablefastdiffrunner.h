#pragma once
#include <priv/qsdiffrunneralgo_p.h>
#include <priv/qimmutablefastdiffrunneralgo_p.h>
#include <qimmutablepatchable.h>
#include <functional>
#include <qimmutableconvert.h>

namespace QImmutable {

template <typename T>
class FastDiffRunner {
public:
    FastDiffRunner() {
    }

    QSPatchSet compare(const QList<T>& from, const QList<T>& to) {
        QImmutable::FastDiffRunnerAlgo<T> algo;
        if (m_customConvertor != nullptr) {
            algo.converter = m_customConvertor;
        }
        return algo.compare(from , to);
    }

    bool patch(Patchable *patchable, const QSPatchSet& patches) const
    {
        QVariantMap diff;
        foreach (QSPatch patch, patches) {
            switch (patch.type()) {
            case QSPatch::Remove:
                patchable->remove(patch.from(), patch.count());
                break;
            case QSPatch::Insert:
                patchable->insert(patch.from(), patch.data());
                break;
            case QSPatch::Move:
                patchable->move(patch.from(), patch.to(), patch.count());
                break;
            case QSPatch::Update:
                if (patch.data().size() > 0) {
                    diff = patch.data().at(0).toMap();
                }
                patchable->set(patch.from(), diff);
                break;
            default:
                break;
            }
        }
        return true;
    }

    void setCustomConvertor(const std::function<QVariantMap (T, int)> &customConvertor)
    {
        m_customConvertor = customConvertor;
    }

private:
    std::function<QVariantMap(T, int)> m_customConvertor;

};



}
