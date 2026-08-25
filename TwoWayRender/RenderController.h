#pragma once

#include <QObject>
#include <QImage>
#include <vector>

class Model;
class RasterizationDevice;
class MVPTransformer;
class TextureImage;
class Triangle;

/**
 * 渲染控制器（QML 可访问的总控）
 *
 * 职责：
 *   - 加载模型、建立 CPU 渲染设备
 *   - 持有相机/模型变换参数（当前只有旋转 + 缩放）
 *   - 暴露 Q_INVOKABLE 接口给 QML 调用
 *   - 渲染后发出 imageUpdated() 信号，驱动 CpuRenderView 刷新
 */
class RenderController : public QObject
{
    Q_OBJECT

public:
    explicit RenderController(QObject *parent = nullptr);
    ~RenderController() override;

    // QML 可调用
    Q_INVOKABLE void rotate(float deltaAngle);

    // C++ 侧接口（给 CpuRenderView 用）
    QImage *image() const;

signals:
    void imageUpdated();

private:
    void init();
    void buildTriangles();

    Model *m_model = nullptr;
    RasterizationDevice *m_device = nullptr;
    MVPTransformer *m_transformer = nullptr;
    TextureImage *m_textureImage = nullptr;
    std::vector<Triangle *> m_triangles;

    float m_scale = 2.0f;
};
