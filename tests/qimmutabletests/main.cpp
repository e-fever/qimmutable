#include <QString>
#include <QtTest>
#include <QtQuickTest/quicktest.h>
#include "testrunner.h"
#include "qsyncabletests.h"
#include "benchmarktests.h"
#include "integrationtests.h"
#include "fastdifftests.h"

namespace AutoTestRegister {
    QUICK_TEST_MAIN(QuickTests)
}

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
#include <execinfo.h>
#include <unistd.h>
#include <signal.h>
void handleBacktrace(int sig) {
  void *array[100];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 100);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
#endif

int main(int argc, char* argv[]) {
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    signal(SIGSEGV, handleBacktrace);
#endif
    qputenv("QML_DISABLE_DISK_CACHE", "1");

    QGuiApplication app(argc, argv);
    TestRunner runner;

    runner.add<QSyncableTests>();
    runner.add(QString(SRCDIR));
    runner.add<IntegrationTests>();
    runner.add<FastDiffTests>();
    runner.add<BenchmarkTests>();

    bool error = runner.exec(app.arguments());

    if (!error) {
        qDebug() << "All test cases passed!";
    }

    return error;
}
