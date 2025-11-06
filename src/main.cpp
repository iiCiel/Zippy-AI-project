#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "programcontroller.h"
#include <QQmlContext>

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    ProgramController controller;
    engine.rootContext()->setContextProperty("controller", &controller);

    engine.loadFromModule("cob_zippy_ai", "Main");

    return app.exec();
}
