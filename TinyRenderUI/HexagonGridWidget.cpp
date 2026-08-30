#include "HexagonGridWidget.h"
bool compare(const Hexagon& a, const  Hexagon& b) {

    if (a.m_y != b.m_y && std::abs(a.m_y - b.m_y) > 1) {
        return a.m_y < b.m_y; // 先按y坐标升序排序
    }
    else {
        return a.m_x < b.m_x; // y相同时，按x坐标升序排序
    }

}
void HexagonGridWidget::setFunc(const std::function<void(QColor)>& func_in) {
    m_func = func_in;
}
HexagonGridWidget::HexagonGridWidget(int hexRadius, QWidget* parent) : QWidget(parent), m_hexRadius(hexRadius)
{


    double hexWidth = static_cast<int>(hexRadius * std::sqrt(3));
    setFixedSize(hexWidth * 19, 2 * hexRadius * 16);

    int startX = hexWidth * (9.5);
    int startY = 2 * hexRadius * 6;

    std::vector<QColor> colors = { {0, 0, 128}, {18, 0, 128}, {39, 0, 128}, {61, 0, 128}, {86, 0, 128}, {107, 0, 128}, {128, 0, 128}, {0, 18, 128}, {0, 0, 192}, {36, 0, 192}, {78, 0, 192}, {128, 0, 192}, {163, 0, 192}, {192, 0, 192}, {128, 0, 103}, {0, 39, 128}, {0, 36, 192}, {0, 0, 255}, {70, 0, 255}, {128, 0, 255}, {192, 0, 255}, {255, 0, 255}, {192, 0, 157}, {128, 0, 82}, {0, 61, 128}, {0, 75, 192}, {0, 70, 255}, {87, 87, 255}, {128, 87, 255}, {192, 87, 255}, {255, 87, 255}, {255, 0, 192}, {192, 0, 128}, {128, 0, 59}, {0, 84, 128}, {0, 128, 192}, {0, 128, 255}, {87, 128, 255}, {160, 160, 255}, {192, 160, 255}, {255, 160, 255}, {255, 87, 192}, {255, 0, 128}, {192, 0, 75}, {128, 0, 37}, {0, 105, 128}, {0, 160, 192}, {0, 192, 255}, {87, 192, 255}, {160, 192, 255}, {233, 233, 255}, {255, 233, 255}, {255, 160, 192}, {255, 87, 128}, {255, 0, 66}, {192, 0, 32}, {128, 0, 16}, {0, 128, 128}, {0, 192, 192}, {0, 255, 255}, {87, 255, 255}, {160, 255, 255}, {233, 255, 255}, {255, 255, 255}, {255, 233, 233}, {255, 160, 160}, {255, 87, 87}, {255, 0, 0}, {192, 0, 0}, {128, 0, 0}, {0, 128, 105}, {0, 192, 160}, {0, 255, 192}, {87, 255, 192}, {160, 255, 192}, {233, 255, 233}, {255, 255, 233}, {255, 192, 160}, {255, 128, 87}, {255, 66, 0}, {192, 32, 0}, {128, 16, 0}, {0, 128, 84}, {0, 192, 128}, {0, 255, 128}, {87, 255, 128}, {160, 255, 160}, {192, 255, 160}, {255, 255, 160}, {255, 192, 87}, {255, 128, 0}, {192, 75, 0}, {128, 37, 0}, {0, 128, 61}, {0, 192, 75}, {0, 255, 70}, {87, 255, 87}, {128, 255, 87}, {192, 255, 87}, {255, 255, 87}, {255, 192, 0}, {192, 128, 0}, {128, 59, 0}, {0, 128, 39}, {0, 192, 36}, {0, 255, 0}, {70, 255, 0}, {128, 255, 0}, {192, 255, 0}, {255, 255, 0}, {192, 157, 0}, {128, 82, 0}, {0, 128, 18}, {0, 192, 0}, {36, 192, 0}, {78, 192, 0}, {128, 192, 0}, {163, 192, 0}, {192, 192, 0}, {128, 103, 0}, {0, 128, 0}, {18, 128, 0}, {39, 128, 0}, {61, 128, 0}, {86, 128, 0}, {107, 128, 0}, {128, 128, 0}, };
    std::vector<QColor> colordown = { {225, 225, 225}, {192, 192, 192}, {166, 166, 166}, {134, 134, 134}, {105, 105, 105}, {75, 75, 75}, {45, 45, 45} };
    std::vector<QColor> colorup = { {212, 212, 212}, {180, 180, 180}, {150, 150, 150}, {120, 120, 120}, {90, 90, 90}, {61, 61, 61}, {29, 29, 30} };


    getCircleCenterPoints(startX, startY, hexRadius);
    std::sort(m_Hexagons.begin(), m_Hexagons.end(), compare);
    int number = 0;
    for (int i = 0; i < 127; ++i) {
        m_Hexagons[i].setColor(colors[i]);
    }

    startX = 2 * hexWidth;
    startY = 7 * hexRadius * 2 + 6 * hexRadius + 7 * hexRadius;
    Hexagon big1 = Hexagon(startX, startY);
    big1.setPolygon(fromCenterGetVertex(startX, startY, hexRadius * 2));
    big1.setColor(QColor(255, 255, 255));
    m_Hexagons.push_back(big1);

    startX = startX + 5 * hexRadius;
    startY = startY + hexRadius;
    for (int i = 0; i < 7; ++i) {

        Hexagon middle = Hexagon(startX, startY);
        middle.setPolygon(fromCenterGetVertex(startX, startY, hexRadius * 1.3));
        middle.setColor(colordown[i]);
        Hexagon middle1 = Hexagon(startX + hexRadius * 1.3 * std::sqrt(3) / 2, startY - hexRadius * 1.3 / 2 - hexRadius * 1.3);
        middle1.setColor(colorup[i]);
        middle1.setPolygon(fromCenterGetVertex(startX + hexRadius * 1.3 * std::sqrt(3) / 2, startY - hexRadius * 1.3 / 2 - hexRadius * 1.3, hexRadius * 1.3));
        m_Hexagons.push_back(middle);
        m_Hexagons.push_back(middle1);
        startX += hexRadius * 1.3 * std::sqrt(3);

    }

    startX = 2 * hexWidth + 5 * hexRadius + 6 * hexRadius * 1.3 * std::sqrt(3) + 6 * hexRadius;
    startY = 7 * hexRadius * 2 + 6 * hexRadius + 7 * hexRadius;
    Hexagon big2 = Hexagon(startX, startY);
    big2.setPolygon(fromCenterGetVertex(startX, startY, hexRadius * 2));
    big2.setColor(QColor(0, 0, 0));
    m_Hexagons.push_back(big2);


}

void HexagonGridWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (auto& it : m_Hexagons) {
        drawHexagon(painter, it, m_hexRadius);
    }
}

void HexagonGridWidget::mousePressEvent(QMouseEvent* event)
{
    QPointF clickPos = event->pos();
    for (auto& points : m_Hexagons) {

        if (points.isContain(clickPos)) {
            points.isClicked = true;
            update();
            if (m_func) {
                m_func(points.m_color);
            }

            emit pickColor(points.m_color);
        };
    }
}

QPolygonF HexagonGridWidget::fromCenterGetVertex(double centerX, double centerY, int radius)
{
    QPolygonF hexagon;
    hexagon << QPointF(centerX, centerY - radius)
            << QPointF(centerX - radius * std::sqrt(3) / 2, centerY - radius / 2)
            << QPointF(centerX - radius * std::sqrt(3) / 2, centerY + radius / 2)
            << QPointF(centerX, centerY + radius)
            << QPointF(centerX + radius * std::sqrt(3) / 2, centerY + radius / 2)
            << QPointF(centerX + radius * std::sqrt(3) / 2, centerY - radius / 2);
    return hexagon;
}

void HexagonGridWidget::drawHexagon(QPainter& painter, Hexagon& hexagon, int radius)
{
    painter.setBrush(hexagon.m_color);
    if (hexagon.isClicked == true) {
        QPen pen(Qt::black, 4); // 5 是边框的粗细
        painter.setPen(pen);
        hexagon.isClicked = false;
    }
    else {
        QPen pen(Qt::black, 1); // 5 是边框的粗细
        painter.setPen(pen);
    }
    painter.drawPolygon(hexagon.m_polygon);
}

std::vector<QPointF> HexagonGridWidget::getCircleHexCenter(int centerX, int centerY, int radius)
{
    radius += 0;
    double newradius = radius * std::sqrt(3) / 2 * 2;
    std::vector<QPointF> container;
    container.push_back(QPointF(centerX + newradius * 1 / 2, centerY - newradius * std::sqrt(3) / 2));
    container.push_back(QPointF(centerX + newradius, centerY));
    container.push_back(QPointF(centerX + newradius * 1 / 2, centerY + newradius * std::sqrt(3) / 2));
    container.push_back(QPointF(centerX - newradius * 1 / 2, centerY + newradius * std::sqrt(3) / 2));
    container.push_back(QPointF(centerX - newradius, centerY));
    container.push_back(QPointF(centerX - newradius * 1 / 2, centerY - newradius * std::sqrt(3) / 2));

    return container;
}

std::vector<QPointF> HexagonGridWidget::getCircleCenterPoints(int CenterX, int CenterY, int circles)
{
    std::vector<QPointF> ans;

    std::queue<QPointF> queuePoints;
    queuePoints.push(QPointF(CenterX, CenterY));
    ans.push_back(QPointF(CenterX, CenterY));
    m_Hexagons.push_back(Hexagon(CenterX, CenterY));
    m_Hexagons[m_Hexagons.size() - 1].setPolygon(fromCenterGetVertex(CenterX, CenterY, m_hexRadius));
    int number = 1;
    int current = 0;
    while (number--) {
        QPointF frontPoints = queuePoints.front();
        queuePoints.pop();
        std::vector<QPointF> temp = getCircleHexCenter(frontPoints.x(), frontPoints.y(), m_hexRadius);
        for (const auto& it : temp) {
            bool isfind = false;
            for (const auto& points : ans) {
                if (std::abs(points.x() - it.x()) < m_hexRadius && std::abs(points.y() - it.y()) < m_hexRadius) {
                    isfind = true;
                    break;
                }
            }
            if (!isfind) {
                ans.push_back(it);
                m_Hexagons.push_back(Hexagon(it.x(), it.y()));
                m_Hexagons[m_Hexagons.size() - 1].setPolygon(fromCenterGetVertex(it.x(), it.y(), m_hexRadius));
                queuePoints.push(it);
            }
        }
        if (number == 0) {
            ++current;
            number = queuePoints.size();
        }

        if (current == 6) {  //the number of circle
            break;
        }


    }

    return ans;
}

