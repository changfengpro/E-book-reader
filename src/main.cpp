#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "book/LibraryController.h"
#include "reader/ReaderController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<LibraryController>("EbookReader.Backend", 1, 0, "LibraryController");
    qmlRegisterType<ReaderController>("EbookReader.Backend", 1, 0, "ReaderController");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("EbookReader", "Main");

    return app.exec();
}
