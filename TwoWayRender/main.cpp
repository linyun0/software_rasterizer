#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
#include <QtQml>

#include "RenderController.h"
#include "CpuRenderView.h"

int main(int argc, char *argv[])
{
    // 设置 OpenGL 3.3 Core（为后续 OpenGL 渲染管线做准备）
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);

    // 注册 C++ 类型给 QML
    qmlRegisterType<RenderController>("TwoWayRender", 1, 0, "RenderController");
    qmlRegisterType<CpuRenderView>("TwoWayRender", 1, 0, "CpuRenderView");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
