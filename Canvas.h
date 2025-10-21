
#pragma once

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <vector>
#include <atomic>

using namespace std; 

class Canvas : public QWidget{
    Q_OBJECT
public: 
    Canvas(vector<atomic<float>> *temp = nullptr,
    vector<QPointF> *positions = nullptr,
    vector<QColor> *colors = nullptr,
    QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    vector<atomic<float>> *temp;
    vector<QPointF> *positions;
    vector<QColor> *colors;
};