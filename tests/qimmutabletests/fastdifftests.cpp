#include <QQmlApplicationEngine>
#include <QTest>
#include <qimmutablevariantlistmodel.h>
#include "immutabletype1.h"
#include "immutabletype2.h"
#include "fastdifftests.h"
#include "priv/qimmutableitem_p.h"
#include "priv/qimmutablefastdiffrunneralgo_p.h"
#include "priv/qimmutablecollection.h"
#include "immutabletype3.h"
#include "qsfastdiffrunner.h"
#include "qimmutablelistmodel.h"

using namespace QImmutable;

template <typename T>
QVariantList convertList(QList<T> list) {
    Item<T> wrapper;
    QVariantList res;
    for (int i = 0 ; i < list.size();i++) {
        res << QImmutable::convert(list[i]);
    }
    return res;
};

FastDiffTests::FastDiffTests(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
          QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void FastDiffTests::test_QImmutable_Collection()
{
    {
        QList<ImmutableType1> list;
        ImmutableType1 v1;
        v1.setId("1");
        list << v1 << ImmutableType1();

        Collection<ImmutableType1> collection(list);
        QCOMPARE(collection.size(), 2);

        QCOMPARE(collection.get(0).id() , QString("1"));
    }

    {
        QQmlApplicationEngine engine;
        QJSValue list = engine.newArray(10);
        list.setProperty("0", engine.toScriptValue<int>(1));
        list.setProperty("1", engine.toScriptValue<int>(2));

        Collection<QJSValue> collection(list);

        QCOMPARE(collection.size(), 10);
        QCOMPARE(collection.get(0).toInt() , 1);
        QCOMPARE(collection.get(1).toInt() , 2);
    }


}

void FastDiffTests::test_QSImmutable_wrapper()
{

    {
        Item<ImmutableType1> wrapper1;
        Item<ImmutableType2> wrapper2;
        Item<ImmutableType3> wrapper3;

        ImmutableType1 v1, v2;
        ImmutableType3 v3;

        QVERIFY(!wrapper1.isShared(v1,v2));
        v1 = v2;
        QVERIFY(wrapper1.isShared(v1,v2));

        v1.setId("a");
        QVariantMap map = QImmutable::convert(v1);
        QVERIFY(map["id"] == v1.id());

        QCOMPARE(wrapper1.hasKey(), true);
        QCOMPARE(wrapper2.hasKey(), false);

        v3.setValue(10);

        QCOMPARE(wrapper1.key(v1), QString("a"));
        QCOMPARE(wrapper3.key(v3), QString("10"));

        v2.setId("b");
    }

    {
        Item<QVariantMap> wrapper;
        QVariantMap v1, v2;
        v1["value1"] = 9;
        QVERIFY(!wrapper.isShared(v1,v2));
        v1 = v2;
        QVERIFY(wrapper.isShared(v1,v2));

        v1["value1"] = 10;
        QVariantMap map = QImmutable::convert(v1);
        QVERIFY(map == v1);

        QVERIFY(!wrapper.hasKey());
        wrapper.keyField = "value1";
        QVERIFY(wrapper.hasKey());
        QCOMPARE(wrapper.key(v1), QString::number(10));
    }

    {
        QQmlApplicationEngine engine;
        Item<QJSValue> wrapper;
        wrapper.keyField = "id";
        QJSValue v1, v2;
        QVERIFY(!wrapper.isShared(v1, v2));
        QVariantMap data;
        data["value1"] = 1;
        data["value2"] = "2";
        v1 = engine.toScriptValue(data);
        v2 = engine.toScriptValue(data);
        QVERIFY(!wrapper.isShared(v1, v2));
        v1 = v2;
        QVERIFY(wrapper.isShared(v1, v2));


    }
}

void FastDiffTests::test_compare()
{
    QVariant v1,v2;

    {
        ImmutableType1 t1a,t1b;

        QVERIFY(!QImmutable::isShared(t1a,t1b));
        t1a.setId("3");
        QVERIFY(!QImmutable::isShared(t1a,t1b));
        t1b.setId("3");
        QVERIFY(!QImmutable::isShared(t1a,t1b));
        t1a = t1b;
        QVERIFY(QImmutable::isShared(t1a,t1b));

    }

    {
        ImmutableType1 t1a,t1b;
        v1 = QVariant::fromValue<ImmutableType1>(t1a);
        v2 = QVariant::fromValue<ImmutableType1>(t1b);

        QVERIFY(!QImmutable::fastCompare(v1, v2));
        t1a = t1b;

        v1 = QVariant::fromValue<ImmutableType1>(t1a);
        QVERIFY(v1 == v2);
        QVERIFY(QImmutable::fastCompare(v1, v2));
    }

    {
        // QList
        ImmutableType1 t1a,t1b, t1c;
        QList<ImmutableType1> l1,l2;
        l1 << t1a << t1b << t1c;
        l2 << t1a << t1b << t1c;
        v1 = QVariant::fromValue<QList<ImmutableType1>>(l1);
        v2 = QVariant::fromValue<QList<ImmutableType1>>(l2);

        QVERIFY(!QMetaType::hasRegisteredComparators(v1.userType()));

        QVERIFY(!QImmutable::fastCompare(v1, v2));
        l1 = l2;
        v1 = QVariant::fromValue<QList<ImmutableType1>>(l1);
        QVERIFY(QImmutable::fastCompare(v1, v2));
    }

}

void FastDiffTests::test_FastDiffRunner()
{
    QFETCH(QList<ImmutableType1>, previous);
    QFETCH(QList<ImmutableType1>, current);
    QFETCH(QList<QSPatch>, changes);

    QList<QSPatch> expectedChanges = changes;

    FastDiffRunner<ImmutableType1> runner;

    QList<QSPatch> patches = runner.compare(previous, current);

    if (changes.size() != patches.size()) {
        qDebug() << expectedChanges;
        qDebug() << patches;
    }

    QCOMPARE(expectedChanges.size(), patches.size());

    for (int i = 0; i < changes.size(); i++) {
        QSPatch expected = changes.at(i);
        QSPatch real = patches.at(i);
        if (!(expected == real))  {
            qDebug() << "Expected" << expectedChanges;
            qDebug() << "Actual" << patches;
        }
        QVERIFY(expected == real);
    }

    {
        QImmutable::ListModel<ImmutableType1> model;
        model.setSource(previous);
        model.setSource(current);

        QVariant currentList = convertList(current);

        if (currentList != model.storage()) {
            qDebug() << "from" << convertList(previous);
            qDebug() << "to" << currentList;
            qDebug() << patches;
        }

        QVERIFY(currentList == model.storage());
    }

}

void FastDiffTests::test_FastDiffRunner_data()
{
    QTest::addColumn<QList<ImmutableType1>>("previous");
    QTest::addColumn<QList<ImmutableType1>>("current");
    QTest::addColumn<QList<QSPatch> >("changes");

    QList<ImmutableType1> previous;
    QList<ImmutableType1> current;
    QList<QSPatch> changes;

    QSPatch c1;

    ImmutableType1 a,b,c,d,e,f;
    a.setId("a");
    b.setId("b");
    c.setId("c");
    d.setId("d");
    e.setId("e");
    f.setId("f");

    /* End of preparation */

    previous << a << b << c;
    current << a << b << c;

    QTest::newRow("No Changes") << previous << current << changes;

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 0, 1);
    QTest::newRow("Remove first element") << previous << current << changes;

    /* Remove all element */
    previous.clear();current.clear();changes.clear();
    c1 = QSPatch();
    previous << a << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 2, 3);
    QTest::newRow("Remove all element") << previous << current << changes;

    /* Remove two elements from different position*/
    previous.clear();current.clear();changes.clear();
    previous << a << b << c << d << e;
    current << a << c << e;
    changes << QSPatch(QSPatch::Remove, 1, 1, 1)
            << QSPatch(QSPatch::Remove, 2, 2, 1);
    QTest::newRow("Remove two elements from different position") << previous << current << changes;

    previous.clear();current.clear();changes.clear();

    current << a << b << c;
    changes << QSPatch(QSPatch::Insert,0,2,3, convertList(QList<ImmutableType1>() << a << b << c));
    QTest::newRow("Add 3 elements to empty list") << previous << current << changes;

    /* Add new element to end */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << b << c << d;
    changes << QSPatch(QSPatch::Insert, 3, 3, 1, QImmutable::convert(d));
    QTest::newRow("Add new element to end") << previous << current << changes;

    /* Add 2 elements to middle */

    previous.clear();current.clear();changes.clear();
    previous << a << b;
    current << a << c << d << b;
    changes << QSPatch(QSPatch::Insert, 1, 2, 2, convertList(QList<ImmutableType1>() << c << d));
    QTest::newRow("Add 2 elements to middle") << previous << current << changes;

    /* Add 2 elements to differnt block */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << d << b << e << c;
    changes << QSPatch(QSPatch::Insert, 1, 1, 1, QImmutable::convert(d))
            << QSPatch(QSPatch::Insert, 3, 3, 1, QImmutable::convert(e));
    QTest::newRow("Add 2 elements to differnt block") << previous << current << changes;

    /* Move from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move from last to first") << previous << current << changes;


    /* Move 2 from first to last */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move 2 from first to last") << previous << current << changes;

    /* Move 2 from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << b << c << a;
    changes.clear();
    changes << QSPatch(QSPatch::Move,1,0,2);

    QTest::newRow("Move 2 from last to first") << previous << current << changes;

    /* Move 2 from last to middle */
    previous.clear();
    previous << a << b << c << d;
    current.clear();
    current << a << c << d << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,1,2);

    QTest::newRow("Move 2 from last to middle") << previous << current << changes;

    /* Remove, Insert, Move */
    previous.clear();
    current.clear();
    changes.clear();

    previous << a << b << c << d << e;
    current << a << f << d << e << c;
    changes << QSPatch(QSPatch::Remove,1,1,1)
            << QSPatch(QSPatch::Insert,1,1,1,QImmutable::convert(f))
            << QSPatch(QSPatch::Move,3,2,2);
    QTest::newRow("Remove, Insert, Move") << previous << current << changes;

    /* Update 2 elements */

    changes.clear();
    previous.clear();
    previous << a << b << c;
    a.setValue("a");
    b.setValue("b");
    current.clear();
    current << a << b << c;

    QVariantMap tmp;
    tmp["value"] = "a";

    changes.clear();
    changes << QSPatch(QSPatch::Update,0,0,1,tmp);

    tmp["value"] = "b";
    changes  << QSPatch(QSPatch::Update,1,1,1,tmp);

    QTest::newRow("Update 2 elements") << previous << current << changes;
}

void FastDiffTests::test_FastDiffRunner_QJSValue()
{
    QQmlApplicationEngine engine;

    FastDiffRunnerAlgo<QJSValue> runner;
    Item<QJSValue> wrapper;
    wrapper.keyField = "key";

    QStringList letters;
    letters << "a" << "b" << "c";
    QJSValueList objects;

    foreach (QString letter, letters) {
        QJSValue object = engine.newObject();
        object.setProperty("key", letter);
        objects << object;
    }

    QJSValue a,b,c;
    a = objects[0];
    b = objects[1];
    c = objects[2];

    QJSValue previous = engine.newArray(3);
    previous.setProperty(0, a);
    previous.setProperty(1, b);
    previous.setProperty(2, c);

    QJSValue current = engine.newArray(2);
    current.setProperty(0, a);
    current.setProperty(1, b);

    QList<QSPatch> patches;
    patches = runner.compare(previous, current);
    QCOMPARE(patches.size(), 1);

    runner.setWrapper(wrapper);
    patches = runner.compare(previous, current);
    QCOMPARE(patches.size(), 1);
}

void FastDiffTests::test_ListModel_setCustomConvertor()
{
    QImmutable::ListModel<ImmutableType1> listModel;

    ImmutableType1 a,b,c;
    a.setId("a");
    b.setId("b");
    c.setId("c");

    QList<ImmutableType1> list;
    list << a << b << c;
    listModel.setCustomConvertor([=](const ImmutableType1& type, int index) {

        QVariantMap map = QImmutable::convert(type);
        map["customValue"] = index;
        return map;
    });

    listModel.setSource(list);

    QCOMPARE(listModel.count(), 3);
    QCOMPARE(listModel.get(0)["customValue"].toInt(), 0);
    QCOMPARE(listModel.get(1)["customValue"].toInt(), 1);
    QCOMPARE(listModel.get(2)["customValue"].toInt(), 2);

}
