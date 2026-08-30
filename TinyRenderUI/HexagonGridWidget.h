#ifndef HEXAGONGRIDWIDGET_H
#define HEXAGONGRIDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include <cmath>
#include <vector>
#include <queue>
#include <algorithm>



struct  Hexagon {

    double m_x;
    double m_y;
    QColor m_color;
    bool isClicked;
    Hexagon(double x = 0, double y = 0, QColor color = QColor(0, 255, 0)) :m_x(x), m_y(y), m_color(color) {
        isClicked = false;
    }
    QPolygonF m_polygon;
    void setPolygon(const QPolygonF polygon) {
        m_polygon = polygon;
    }
    void setColor(const QColor& color) {
        m_color = color;
    }

    bool isContain(QPointF pos) {
        QPointF point1 = m_polygon.at(0);
        QPointF point2 = m_polygon.at(1);
        QPointF point3 = m_polygon.at(2);
        QPointF point4 = m_polygon.at(3);
        QPointF point5 = m_polygon.at(4);
        QPointF point6 = m_polygon.at(5);

        QPointF vec = point2 - point1;
        bool is = false;
        is = crossProduct(vec, pos - point1);
        if (is) {
            return false;
        }

        vec = point3 - point2;
        if (is != crossProduct(vec, pos - point2)) {
            return false;
        }

        vec = point4 - point3;
        if (is != crossProduct(vec, pos - point3)) {
            return false;
        }

        vec = point5 - point4;
        if (is != crossProduct(vec, pos - point4)) {
            return false;
        }
        vec = point6 - point5;
        if (is != crossProduct(vec, pos - point5)) {
            return false;
        }
        vec = point1 - point6;
        if (is != crossProduct(vec, pos - point6)) {
            return false;
        }



        return true;
    }

    bool crossProduct(QPointF A, QPointF B) {
        return (A.x() * B.y() - A.y() * B.x()) > 0 ? true : false;
    }
};




class HexagonGridWidget : public QWidget {
    Q_OBJECT
signals:
    void pickColor(QColor);

public:
    HexagonGridWidget(int hexRadius = 14, QWidget* parent = nullptr);

    void setFunc(const std::function<void(QColor)>& func_in);
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event);
private:
    QPolygonF  fromCenterGetVertex(double centerX, double centerY, int radius);
    void drawHexagon(QPainter& painter, Hexagon& hexagon, int radius);
    std::vector<QPointF> getCircleHexCenter(int centerX, int centerY, int radius);
    std::vector<QPointF> getCircleCenterPoints(int CenterX, int CenterY, int circles);
private:
    std::function<void(QColor)> m_func;
private:
    std::vector<Hexagon> m_Hexagons;

    int m_hexRadius; // Radius of a single hexagon side
};


#endif
