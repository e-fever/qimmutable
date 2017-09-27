#ifndef QSYNCABLETESTS_H
#define QSYNCABLETESTS_H

#include <QObject>

class QSyncableTests : public QObject
{
    Q_OBJECT
public:
    explicit QSyncableTests(QObject *parent = 0);

signals:

public slots:

private slots:
    void patch();
    void patch_merge();

    void tree();
    void tree_insert();
    void tree_remove();
    void tree_updateMin();
    void tree_balance();

    void test_ListModel_move();
    void test_ListModel_move_data();

    void test_ListModel_failedCase();
    void test_ListModel_failedCase_data();

    void test_ListModel_noKeyField();
    void test_ListModel_noKeyField_data();

    void diffRunner_invalidKey();

    void diffRunner_random();
    void diffRunner_randomMove();

    void diffRunner_complex();
    void diffRunner_complex_data();

//    void listModel_insert();
    void listModel_roleNames();

};

#endif // QSYNCABLETESTS_H
