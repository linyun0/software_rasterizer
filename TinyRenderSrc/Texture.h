//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
//#include "global.hpp"
#include <Eigen/Eigen>
#include <QImage.h>
#include <QFile.h>

class TextureImage {
private:
    QImage* m_Image=nullptr;

public:
    TextureImage(const std::string& name)
    {
     m_Image = new QImage(QString(name.c_str()));
     width = m_Image->width();
     height = m_Image->height();
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = ( v) * height;
        if (u_img < 0 || v_img < 0) {
            return Eigen::Vector3f();
        }

        QRgb rgb = m_Image->pixel(u_img, v_img);

        // 提取RGB分量
        int red = qRed(rgb);
        int green = qGreen(rgb);
        int blue = qBlue(rgb);
        return Eigen::Vector3f(red, green, blue);
    }

};
#endif //RASTERIZER_TEXTURE_H
