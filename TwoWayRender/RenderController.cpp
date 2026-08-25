#include "RenderController.h"

#include "DataLoader.h"
#include "RasterizationDevice.h"
#include "MVPTransformer.h"
#include "Texture.h"
#include "Triangle.h"
#include "geometry.h"

#include <QSize>
#include <QDebug>
#include <iostream>

RenderController::RenderController(QObject *parent)
    : QObject(parent)
{
    init();
}

RenderController::~RenderController()
{
    for (auto *t : m_triangles)
        delete t;
    m_triangles.clear();

    delete m_device;
    delete m_transformer;
    delete m_textureImage;
    delete m_model;
}

void RenderController::init()
{
    // 1. 加载模型（Assimp）
    m_model = new Model();

    // 2. 建立 MVP 变换器
    m_transformer = new MVPTransformer();

    // 3. 从 Mesh 顶点造 Triangle 列表（CPU 光栅化的输入）
    buildTriangles();

    // 4. 建立 CPU 光栅化设备
    m_device = new RasterizationDevice(QSize(700, 700));
    m_device->SetTriangles(m_triangles);
    m_device->SetMVPTransformer(m_transformer);

    // 5. 加载纹理（与模型同目录）
    m_textureImage = new TextureImage(m_model->directory + "/spot_texture.png");
    m_device->SetTextureImage(m_textureImage);

    std::cerr << "[RenderController] 模型加载完成:"
              << " mesh 数 = " << m_model->meshes.size()
              << ", 三角形数 = " << m_triangles.size()
              << ", 纹理尺寸 = " << m_textureImage->width << "x" << m_textureImage->height
              << std::endl;

    // 6. 渲染第一帧
    m_device->Draw();
}

void RenderController::buildTriangles()
{
    for (const auto &mesh : m_model->meshes) {
        for (size_t i = 0; i + 2 < mesh.m_vertices.size(); i += 3) {
            Triangle *t = new Triangle();
            for (int j = 0; j < 3; ++j) {
                const auto &v = mesh.m_vertices[i + j];
                t->setVertex(j, Eigen::Vector4f(v.Position.x, v.Position.y, v.Position.z, 1.0f));
                t->setNormal(j, Eigen::Vector3f(v.Normal.x, v.Normal.y, v.Normal.z));
                t->setTexCoord(j, Eigen::Vector2f(v.TexCoords.x, v.TexCoords.y));
            }
            m_triangles.push_back(t);
        }
    }
}

void RenderController::rotate(float deltaAngle)
{
    if (!m_transformer || !m_device)
        return;

    // MVPTransformer::SetModelArc 是增量式旋转
    m_transformer->SetModelArc(deltaAngle, m_scale);
    m_device->Draw();

    emit imageUpdated();
}

QImage *RenderController::image() const
{
    return m_device ? m_device->GetImage() : nullptr;
}
