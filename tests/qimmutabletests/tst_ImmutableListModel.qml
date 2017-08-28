import QtQuick 2.0
import QtTest 1.0
import QSyncable 1.0
import QImmutable 1.0

Item {
    width: 640
    height: 480

    TestCase {
        name: "ImmutableListModel"
        when: windowShown

        Component {
            id: creator;
            ImmutableListModel {
                id: model
            }
        }

        function test_basic() {
            var a = { key: "a"}
            var b = { key: "b"}
            var c = { key: "c"}

            var previous = [a,b,c];
            var current = [b,c];

            var model = creator.createObject();
            model.keyField = "key";
            model.source = previous;
            compare(model.count, 3);
            compare(model.get(0).key, "a");
            compare(model.get(1).key, "b");
            compare(model.get(2).key, "c");

            model.source = current;
            compare(model.count, 2);
            compare(model.get(0).key, "b");
            compare(model.get(1).key, "c");

            model.destroy();
        }

    }
}

