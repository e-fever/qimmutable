import QtQuick 2.0
import QtTest 1.0
import QSyncable 1.0
import QImmutable 1.0

Item {
    id: root
    width: 640
    height: 480

    TestCase {
        name: "ImmutableListModelNested"
        when: windowShown

        Component {
            id: creator

            Repeater {
                id: repeater
                model: ImmutableListModel {
                    id: immutableListModel
                    fields: [
                        "key",
                        "value"
                    ]
                }

                delegate: Item {
                    property var value: model.value
                    property var listModel: ImmutableListModel {
                        source: model.value
                    }
                }
            }
        }

        function test_basic() {
            var repeater = creator.createObject(root);
            var a = { key: "a"}
            var b = { key: "b"}
            var c = { key: "c"}
            var d = { key: "d"}
            var e = { key: "e"}
            var f = { key: "f"}
            var deList = [d,e];
            var fList = [f];

            a.value = deList;
            b.value = fList;
            c.value = [];
            var list = [a,b,c];
            repeater.model.source = list;
            compare(repeater.model.get(0).key, "a");
            compare(repeater.model.get(0).value === deList, true);
            compare(repeater.model.count, 3);
            compare(repeater.count, 3);
            var item = repeater.itemAt(0);
            compare(item.listModel.source === deList, true);

            repeater.destroy();
        }

    }

}
