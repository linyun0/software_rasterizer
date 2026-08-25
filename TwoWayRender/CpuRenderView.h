#pragma once

#include <QQuickPaintedItem>
#include "RenderController.h"

/**
 * CPU 渲染视图（QML 类型）
 *
 * 用 QPainter 把 RenderController 渲染出的 QImage 画到界面上。
 * 监听 RenderController 的 imageUpdated() 信号自动刷新。
 */
class CpuRenderView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(RenderController *controller READ controller WRITE setController NOTIFY controllerChanged)

public:
    explicit CpuRenderView(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

    RenderController *controller() const;
    void setController(RenderController *controller);

signals:
    void controllerChanged();

private:
    RenderController *m_controller = nullptr;
};
