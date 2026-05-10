#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "reader/ReaderController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

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
