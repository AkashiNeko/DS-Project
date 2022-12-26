#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include "graph.h"

class PaintWidget : public QWidget {
    Q_OBJECT
public:
    explicit PaintWidget(QWidget *parent = nullptr);
    void drawCityInfo(int x, int y, QString cityNum, QString name, QColor color = Qt::white);
    void drawCity(int x, int y, QColor color = {0, 160, 240}, QColor edge = {0, 100, 150});
    void drawGraph();
    void drawDistance(int x1, int y1, int x2, int y2, int distance);
    void drawRoad(int x1, int y1, int x2, int y2, QColor color = {160, 160, 160});
    void drawRoads();
    size_t isOnTheCity(int x1, int y1);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void PaintWidgetMouseClick();
public:
    int r = 25;
    int _x;
    int _y;
    Graph g;
    bool tempCity = false;
    size_t select = (size_t)-1;
    std::vector<size_t> tvsResult;
    size_t tvsStep = 0;
    size_t shortestPathEnd = (size_t)-1;
};

#endif // PAINTWIDGET_H
