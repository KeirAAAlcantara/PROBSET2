#include "canvas.h"
#include <QRectF> 

Canvas::Canvas(vector<atomic<float>> *temp,
                vector<QPointF> *positions,
                vector<QColor> *colors,
                QWidget *parent)
        : QWidget(parent), temp(temp), positions(positions), colors(colors){}

void Canvas::paintEvent(QPaintEvent *){
    if(!temps || !positions || !colors){
        qWarning() << "missing values" ;
        return;
    }
    
    QPainter painter(this);
    painter.setRender(QPainter::Antialiasing)

    int radius = 25;

    for(size_t i=0; i<temp->size(), ++i){
        float temperature = temp->at(i).load();
        QPointF pos = positions->at(i);
        QColor color = colors->at(i);

        QColor fill color = color.lighter(100 + int(temp*2));
        painter.setBrush(fillColor);
        painter.setPen(Qt::black);

        painter.drawEllipse(pos, radius, radius);
        
        QString text = QString::number(temperature, 'f', 1);
        QRectF textRect(pos.x() - radius, pos.y() - radius, radius*2, radius*2);
        painter.drawText(textRect, Qt::AlignCenter, text);
    }
}