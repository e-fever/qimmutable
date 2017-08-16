QImmutable - An Immutable List Model
========================================================

**This project is still under construction**

QImmutable provides an easy to use ListModel for QML. It doesn’t have methods like "insert()"/"remove()"/"move()".  The only way to update the ListModel is the `source` property setter.

Example in C++

```C++
    QList<CustomImmutableType> input;

    QImmutable::ListModel<CustomImmutableType> listModel;

    /* Set initial data*/
    listModel.setSource(input);

    /* Insert a new item at the beginning */

    input.insert(0, CustomImmutableType());

    listModel.setSource(input);

    // It will be updated to the latest version
    // and emit rowsInserted() signal.
```

When the setter is called, it will perform a synchronization by comparing with the previous input then finding out the diff. It will generate a list of change operations like insertion, removal and move by an average O(n) algorithm. And apply the changes to itself. It will guarantee the behaviour is almost identical to the original QML ListModel. Such that the UI components could react to the changes correctly.

The data in QImmutable must be immutable (implicitly shared class). It could avoid unnecessary clone of data for more effective memory management. Moreover, determine does the data changed between immutable objects is as fast as to compare pointer. QImmutable make sure of immutable’s feature to achieve a faster synchonization.

An immediate benefit of using QImmutable is the simplification of the data pipeline. If you need your UI to respond to changes like insertion/removal correctly, you must update the ListModel by the corresponding method explicitly. QImmutable combines all kinds of update methods into a single way. User doesn’t need to care about their differences and setup data binding by just a single connection.

Moreover, QImmutable could also be used as a solution for the nested list model.

Reference: 

1. [Efficient models for QML with QSyncable - Google Slides](https://docs.google.com/presentation/d/13pkRav2Fks_AKTXfKtGTyBuc_RmkZGiDRQZTr4usQFk/pub?start=false&loop=false&delayms=3000&slide=id.p)

How does it work?
-----------------

![Workflow](https://raw.githubusercontent.com/benlau/junkcode/master/docs/qimmutable-workflow.png)

**DiffRunner (QSDiffRunner)** compares two QList<T> with immutable type to produce a patch for transforming one of a list to another list with minimum no. of steps. The result can be applied on a VariantListModel. DiffRunner uses an average O(n) algorithm and therefore it should be fast enough for regular UI application.

**VariantListModel** is an implementation of QAbstactItemModel. It stores data in a list of QVariantMap. It will emit insert, remove, move and data changed signals according to the patch applied.

QImmutable provides the two classes above for a user to convert their own data structure to a QML friendly list model. Usually, there are several ways to update a list model. QImmutable combines all of the update methods into a single process - patching.

Whatever the data source has been changed, regardless of update method, the user converts it into a QVariantList and pass it to DiffRunner to compare with the current snapshot. Then apply the generated patch on QSListModel. UI component will be refreshed according to the emitted signals from QSListModel.

The diagram below shows an example application architecture using QImmutable:

![QSyncable Application Architecture](https://raw.githubusercontent.com/benlau/qsyncable/master/docs/qsyncable-application-architecture-example.png)

In QImmutable application, ListModel only keeps a copy of the data. it is meaningless for UI components to modify it. Instead, UI components should ask to update the data source and trigger synchronization afterwards. The component for “updates” and “queries” is in fact separated. (More information in this [article](https://medium.com/@benlaud/action-dispatcher-design-pattern-for-qml-c350b1d2a7e7#.mi3b8hbuv) )

Immutable and Implicit Sharing
------------------------------

An immutable object is an object whose state can not be modified after it is created. With a technique called copy-on-write, when a user asks the system to copy an immutable object, it will create a new reference that still pointing to the same object. Once a user attempts to modify the object, it will make a copy of the entire object and applies the modification to that. The other users are unaffected. It provides a fast and low-cost mechanism for copying data which will not be changed unexpectedly. It is an ideal data structure for data sharing between thread.

Qt’s Implicitly Shared Class is also a kind of immutable data structure supports copy-on-write. QImmutable makes use of the Implicitly Shared Class mechanism to discover unchanged data quickly.

TODO - Instruction


Design Principle - Separation of "updates" and "queries"
----------

QImmutable is designed to solve a fundamental problem of C++ & QML application: How to share data between C++ and QML?

QObject list model is definitely a bad idea. It is terrible to manage their life cycle and ownership (QML / C++ scope). You should be aware of garbage collection in your QObject list model even it is written in C++.

Using a variant list model is better, but it is not C++ friendly. And it is difficult to handle nested list model.

In fact, the problem will be simple if you separate “updates” and “queries” into different components.
First of all, you don’t even need to consider QObject list model approach.
It has no any advantage of using QObject list model if you use another component for the update.First of all, you don’t even need to consider QObject list model approach.
It has no any advantage of using QObject list model if you use other component for update.

Moreover, it is not necessary to use a variant list model as a central data source. You may use any data structure you like. Leave variant list model for presentation only.

QImmutable takes a step further. It simplifies the process to update the variant list model from a data source by combining insertion, removal, move and change operations into a single process - patching, while maintaining the correctness of UI response. It solves not only the problem of C++ and QML data sharing, but also a solution of nested list model within QML.

Reference:

1. [What the Flux? (On Flux, DDD, and CQRS) — Jack Hsu](http://jaysoo.ca/2015/02/06/what-the-flux/)

Why use QImmutable for C++?
--------------------------

(1) The function of QQmlListProperty is limited.

(2) Implement your own QAbstactItemModel is troublesome.

You need to understand how QAbstactItemModel works and emit insert, remove, move and update signals correctly. Otherwise, UI components like ListView will not react correctly.

(3) Use implicit sharing class over QObject

QObject is not copyable and you need to manage its life cycle. It is not really a good solution as a data storage class.

Implicit sharing class is recommended for this purpose. Because it can safely be copied across threads, like any other QVariant classes.
Once you find that your data is too big for processing, you may pass it to a thread and work in the background.

[Implicit Sharing | Qt Core 5.5](http://doc.qt.io/qt-5/implicit-sharing.html)

(4) Works for any data structure

You just need to write a conversion function, QImmutable will do the rest for you.

(5) Simple update method

No matter what kind of update happen, just convert your data structure to QVariantList, pass it to DiffRunner, then patch a model.

Why use QImmutable for QML?
--------------------------

(1) Use JsonListModel to wrap your Javascript object.

(2) Able to work as a nested list model.

(3) Simple data pipeline.

Algorithm
---------

The average time complexity of the diff algorithm in QSDiffRunner is O(n), where n is the sum of the number of items in comparison.
But it is not the worst case time complexity. It is O(n + m log m), where m is the no. of block move operations.

However, unless you are doing a reverse or random shuffle.
Moving a single block of items, regardless of the number, from one to another place will remain in O(n) time complexity.

If you really need to do a reverse of a list or random shuffle with a large amount of data, you may consider to move it to a thread or set the key field to null.
QSDiffRunner will ignore the insertion, removal, and move checking if the key field is not set.

| Condition                    | Time Complexity             |
|------------------------------|-----------------------------|
| Insert an empty list         | O(1)                        |
| Clear the list               | O(1)                        |
| Unchanged list               | O(1)                        |
| Insert items to any position | O(n)                        |
| Remove a block of items      | O(n)                        |
| Move a block of items        | O(n)                        |
| Insert, remove, updates      | O(n)                        |
| Reverse the list             | O(n + m log m)              |
| Random shuffle               | O(n + m log m)              |

Installation
------------

Download a release and bundle the folder within your source tree.

Or:

```
qpm install net.efever.immutablelistmodel
```

Class Reference
---------------

[ImmutableListModel - Class Reference](http://efever.github.io/immutablelistmodel/)

Example
-------

[faketrello](https://github.com/efever/immutablelistmodel/tree/master/examples/faketrello) - Simulate a Trello card board.
