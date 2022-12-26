#include "include/paintwidget.h"
#include <QPainter>
#include <QMouseEvent>

PaintWidget::PaintWidget(QWidget *parent) : QWidget{parent} {
    this->setAttribute(Qt::WA_StyledBackground, true);
}

void PaintWidget::drawGraph() {
    update();
}

void PaintWidget::drawDistance(int x1, int y1, int x2, int y2, int distance) {
    int diffX = x2 - x1;
    int diffY = y2 - y1;
    double dis = sqrt(diffX * diffX + diffY * diffY);
    int x = x2 - diffX * (2.2 * r / dis);
    int y = y2 - diffY * (2.2 * r / dis);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = painter.font();
    font.setPixelSize(0.8 * r);
    painter.setFont(font);
    painter.setPen(QColor(Qt::black));
    painter.drawText(QRect(x - 0.8 * r, y - 0.8 * r, 1.6 * r, 1.6 * r), Qt::AlignVCenter | Qt::AlignHCenter, QString::number(distance));
}

void PaintWidget::drawCityInfo(int x, int y, QString cityNum, QString name, QColor color) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = painter.font();
    font.setPixelSize(r * 1.2 - (cityNum.size() - 1) * cityNum.size());
    painter.setFont(font);
    painter.setPen(QColor(color));
    painter.drawText(QRect(x - r, y - r, 2 * r, 2 * r), Qt::AlignVCenter | Qt::AlignHCenter, cityNum);
    font.setPixelSize(r * 0.6);
    painter.setFont(font);
    painter.setPen(QColor(Qt::red));
    painter.drawText(QRect(x + r * 0.6, y + r * 0.6, 100, 20), Qt::AlignLeft, name);
}

void PaintWidget::drawCity(int x, int y, QColor color, QColor edge) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(edge, r / 12.0, Qt::SolidLine));
    painter.setBrush(QBrush(color, Qt::SolidPattern));
    painter.drawEllipse({x, y}, r, r);
}

void PaintWidget::drawRoad(int x1, int y1, int x2, int y2, QColor color) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(color, r / 8.0, Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.drawLine(x1, y1, x2, y2);
    // trigonometric function
    //double theta = atan(((double)y2 - y1) / (x2 - x1));
    //int dir = x2 < x1 ? -1 : 1;
    //int arrayX = x2 - dir * r * cos(theta);
    //int arrayY = y2 - dir * r * sin(theta);
    //const double PI = 3.1415926536;
    //int m1 = arrayX - dir * r * cos(theta + PI / 6);
    //int n1 = arrayY - dir * r * sin(theta + PI / 6);
    //painter.drawLine(arrayX, arrayY, m1, n1);
    //int m2 = arrayX - dir * r * cos(theta - PI / 6);
    //int n2 = arrayY - dir * r * sin(theta - PI / 6);
    //painter.drawLine(arrayX, arrayY, m2, n2);

    // no trigonometric function
    int diffX = x2 - x1;
    int diffY = y2 - y1;
    double distance = sqrt(diffX * diffX + diffY * diffY);
    double dx1 = diffX * (r / distance);
    double dy1 = diffY * (r / distance);
    int arrayX = x2 - dx1;
    int arrayY = y2 - dy1;
    const static double sqrt3div2 = sqrt(3) / 2;
    double dx2 = diffX * ((1 + sqrt3div2) * r / distance);
    double dy2 = diffY * ((1 + sqrt3div2) * r / distance);
    int centerX = x2 - dx2;
    int centerY = y2 - dy2;
    double k = diffX ? (double)-diffX / diffY : 0;
    double cot = 1 / k;
    double cos = sqrt(1 / (k * k + 1));
    double sin = sqrt(1 / (cot * cot + 1));
    if (k < 0) cos *= -1;
    int rx1 = r / 2 * cos;
    int ry1 = r / 2 * sin;
    painter.drawLine(arrayX, arrayY, centerX + rx1, centerY + ry1);
    painter.drawLine(arrayX, arrayY, centerX - rx1, centerY - ry1);
}

void PaintWidget::drawRoads() {
    for (auto& c : g) {
        if (!c.cityName.empty()) {
            for (auto& nbs : c.neighbors) {
                drawRoad(c.coordinate.x, c.coordinate.y, g[nbs.cityNum].coordinate.x, g[nbs.cityNum].coordinate.y);
            }
        }
    }
    for (auto& c : g) {
        if (!c.cityName.empty()) {
            for (auto& nbs : c.neighbors) {
                drawDistance(c.coordinate.x, c.coordinate.y, g[nbs.cityNum].coordinate.x, g[nbs.cityNum].coordinate.y, nbs.distance);
            }
        }
    }
}

size_t PaintWidget::isOnTheCity(int x, int y) {
    auto sz = g.size();
    for (size_t i = 0; i < sz; ++i) {
        if (g.emptyCity(i)) continue;
        int x0 = g[i].coordinate.x;
        int y0 = g[i].coordinate.y;
        if (sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0)) <= r) {
            return i;
        }
    }
    return (size_t)-1;
}

void PaintWidget::paintEvent(QPaintEvent *event) {
    drawRoads();
    if (tempCity) {
        drawCity(_x, _y, {140, 200, 240}, {110, 150, 170});
        drawCityInfo(_x, _y, "new", "city");
    }
    auto sz = g.size();
    for (size_t i = 0; i < sz; ++i) {
        if (!g[i].cityName.empty()) {
            drawCity(g[i].coordinate.x, g[i].coordinate.y);
            drawCityInfo(g[i].coordinate.x, g[i].coordinate.y, QString::number(i), QString::fromStdString(g[i].cityName));
        }
    }
    if (tvsStep) {
        for (size_t i = 0; i < tvsStep; ++i) {
            auto x = g[tvsResult[i]].coordinate.x;
            auto y = g[tvsResult[i]].coordinate.y;
            drawCity(x, y, {240, 100, 40}, {160, 40, 0});
            drawCityInfo(x, y, QString::number(tvsResult[i]), "", {255,215,202});
        }
    }
    else if (shortestPathEnd != (size_t)-1) {
        auto path = g.getShortestPath(shortestPathEnd);
        auto pathCount = path.size() - 1;
        for (size_t i = 0; i < pathCount; ++i) {
            auto x = g[path[i]].coordinate.x;
            auto y = g[path[i]].coordinate.y;
            auto x1 = g[path[i + 1]].coordinate.x;
            auto y1 = g[path[i + 1]].coordinate.y;
            drawRoad(x, y, x1, y1, Qt::red);
            drawCity(x, y, {255,115,115}, Qt::red);
            drawCityInfo(x, y, QString::number(path[i]), "");
        }
        drawCity(g[*path.rbegin()].coordinate.x, g[*path.rbegin()].coordinate.y, {255,100,200}, {190,55,140});
        drawCityInfo(g[*path.rbegin()].coordinate.x, g[*path.rbegin()].coordinate.y, "END", "");
    }
    if (select != (size_t)-1) {
        drawCity(g[select].coordinate.x, g[select].coordinate.y, {0, 233, 174}, {0, 140, 100});
        drawCityInfo(g[select].coordinate.x, g[select].coordinate.y, QString::number(select), "");
    }
}

void PaintWidget::mousePressEvent(QMouseEvent *event) {
    if(event->buttons()==Qt::LeftButton) {
        int prevX = _x;
        int prevY = _y;
        size_t tmp;
        if ((tmp = isOnTheCity(event->pos().x(), event->pos().y())) != (size_t)-1) {
            tempCity = false;
            select = (select == tmp ? -1 : tmp);
        }
        else {
            select = -1;
            _x = event->pos().x();
            _y = event->pos().y();

            if (tempCity) {
                if (sqrt((prevX - _x) * (prevX - _x) + (prevY - _y) * (prevY - _y)) <= r)
                    tempCity = false;
            }
            else {
                tempCity = true;
            }
        }
        drawGraph();
    }
    PaintWidgetMouseClick();
}
