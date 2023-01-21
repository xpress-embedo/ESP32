#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "someclass.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  qmlRegisterType< SomeClass > ("Monty", 1, 0, "SomeClass");

  QQmlApplicationEngine engine;
  const QUrl url(u"qrc:/MQTT_Application/main.qml"_qs);
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                   &app, [url](QObject *obj, const QUrl &objUrl) {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);

  // this is first method
  /*
  SomeClass testClass;
  QQmlContext * rootContext = engine.rootContext();
  rootContext->setContextProperty("classA", &testClass);
  */

  return app.exec();
}
