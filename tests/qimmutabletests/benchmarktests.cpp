#include <QTest>
#include <QUuid>
#include <QSDiffRunner>
#include "immutabletype1.h"
#include "benchmarktests.h"
#include "qimmutablefastdiffrunner.h"
#include "priv/qimmutableitem_p.h"

static QVariantList create(int size) {

    QVariantList result;
    static int nextId = 0;

    for (int i = 0 ; i < size ; i++) {
        QVariantMap item;
        item["id"] = QString("%1").arg(nextId++);
        item["value"] =  qrand();
        result << item;
    }

    return result;
}

static QList<ImmutableType1> createData(int size) {
    static int nextId = 0;

    QList<ImmutableType1> result;

    for (int i = 0 ; i < size ; i++) {
        ImmutableType1 item;
        item.setId(QString("%1").arg(nextId++));
        item.setValue(QString("%1").arg(qrand() * 10000));
        result << item;
    }

    return result;
}

BenchmarkTests::BenchmarkTests(QObject *parent) : QObject(parent)
{
#if 0
    sizes << 100 << 1000 << 10000 << 100000;
#else
    sizes << 100000;
#endif
    auto ref = [=]() {
          QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void BenchmarkTests::copy()
{
    QVariantList from = create(100000);
    QVariantList to;

    QBENCHMARK {

        for (int i =0 ; i < from.size() ; i++) {
            to << from.at(i);
        }
    }
}

void BenchmarkTests::hashing()
{
    QStringList uuids;
    for (int i = 0 ; i < 100000 ; i++) {
        uuids << QUuid::createUuid().toString().replace(QRegExp("[{}]"), "");
    }

    QBENCHMARK {
        QHash<QString,int> hash;
        hash.reserve(100000 + 100);
        for (int i =0 ; i < 100000 ; i++) {
            hash[uuids.at(i)] = i;
        }
    }
}

void BenchmarkTests::changeAll()
{
    QFETCH(int, size);

    QImmutable::Item<ImmutableType1> wrapper;
    QVERIFY(wrapper.hasKey());

    auto from = createData(size);
    QVERIFY(from.size() == size);

    auto to = from;

    for (int i = 0 ; i < to.count() ; i++) {
        auto item = to[i];
        item.setValue(item.value() + "suffix");
        to[i] = item;
    }

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
//        runner.setKeyField("id");
        runner.compare(from, to);
    }

}

void BenchmarkTests::changeAll_data()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }

}

void BenchmarkTests::removeAll()
{
    QFETCH(int, size);

    auto from = createData(size);
    QVERIFY(from.size() == size);

    QList<ImmutableType1> to;
    QList<QSPatch> patches;

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        patches = runner.compare(from, to);
    }

}

void BenchmarkTests::removeAll_data()
{
    setup();
}

void BenchmarkTests::removeOne()
{
    QFETCH(int, size);

    auto from = createData(size);
    QVERIFY(from.size() == size);

    auto to = from;
    to.takeFirst();
    QList<QSPatch> patches;

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        patches = runner.compare(from, to);
    }

}

void BenchmarkTests::removeOne_data()
{
    setup();
}

void BenchmarkTests::moveAll()
{
    QFETCH(int, size);

    auto from = createData(size);
    QVERIFY(from.size() == size);
    auto to = from;

    for (int i = 0 , k = 1; i < from.size() ; i++ , k++) {
        int j = (i + k) % from.size();
        to.move(j,i);
    }

    QList<QSPatch> patches;

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        patches = runner.compare(from, to);
    }

    qDebug() << "Total no. of moves" << patches.size();
}

void BenchmarkTests::moveAll_data()
{
    setup();
}

void BenchmarkTests::reverse()
{
    QFETCH(int, size);

    auto from = createData(size);
    QVERIFY(from.size() == size);
    QList<ImmutableType1> to;
    to.reserve(from.size());

    for (int i = from.size() - 1 ; i >= 0 ; i--) {
        to << from.at(i);
    }

    QList<QSPatch> patches;

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        patches = runner.compare(from, to);
    }

    QCOMPARE(patches.size() , to.size() - 1);

}

void BenchmarkTests::reverse_data()
{
    setup();
}

void BenchmarkTests::setup()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }

}

void BenchmarkTests::noChange()
{
    QFETCH(int, size);

    auto from = createData(size);
    QVERIFY(from.size() == size);

    auto to = from;
    QVERIFY(QImmutable::isShared(to, from));
    QVERIFY(from.isSharedWith(to));

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        runner.compare(from, to);
    }

}

void BenchmarkTests::noChange_data()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }
}

void BenchmarkTests::changeOne()
{
    QFETCH(int, size);

    auto from = createData(size);
    QVERIFY(from.size() == size);

    auto to = from;
    ImmutableType1 item = to.at(0);
    item.setValue(item.value() + "suffix");
    to[0] = item;

    QVERIFY(!from.isSharedWith(to));

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        runner.compare(from, to);
    }
}

void BenchmarkTests::changeOne_data()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }
}

void BenchmarkTests::moveOne()
{
    QFETCH(int, size);

    auto from = createData(size);
    QVERIFY(from.size() == size);

    auto to = from;
    to.move(0, to.size() - 1);

    QVERIFY(!from.isSharedWith( to));

    QSPatchSet patch;

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        patch = runner.compare(from, to);
    }

    QVERIFY(patch.size() == 1);
}

void BenchmarkTests::moveOne_data()
{
    setup();
}

void BenchmarkTests::insertAll()
{
    QFETCH(int, size);

    auto to = createData(size);
    decltype(to) from;

    QBENCHMARK {
        QImmutable::FastDiffRunner<ImmutableType1> runner;
        runner.compare(from, to);
    }

}

void BenchmarkTests::insertAll_data()
{
    setup();
}

