
#pragma once

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPointF>
#include <vector>
#include <atomic>

using namespace std; 

class Canvas : public QWidget{
    Q_OBJECT
public: 
    Canvas(const vector<shared_ptr<atomic<float>>> *temp,
    const vector<QPointF> *positions,
    const vector<QColor> *colors,
    QWidget *parent = nullptr);

    void updateDisplay();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    const vector<shared_ptr<atomic<float>>> *temp;
    const vector<QPointF> *positions;
    const vector<QColor> *colors;
};