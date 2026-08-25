#include "CpuRenderView.h"
#include "RenderController.h"

#include <QPainter>

CpuRenderView::CpuRenderView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
}

void CpuRenderView::paint(QPainter *painter)
{
    if (m_controller && m_controller->image()) {
        painter->drawImage(boundingRect(), *m_controller->image());
    }
}

RenderController *CpuRenderView::controller() const
{
    return m_controller;
}

void CpuRenderView::setController(RenderController *controller)
{
    if (m_controller == controller)
        return;

    if (m_controller) {
        disconnect(m_controller, &RenderController::imageUpdated, this, nullptr);
    }

    m_controller = controller;

    if (m_controller) {
        connect(m_controller, &RenderController::imageUpdated, this, [this]() {
            update();
        });
    }

    emit controllerChanged();
    update();
}
