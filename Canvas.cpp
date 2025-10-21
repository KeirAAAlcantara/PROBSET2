#include "Canvas.h"
#include <QRectF> 
#include <QDebug>
#include <QPoint> 

Canvas::Canvas(const vector<atomic<float>> *temp,
                const vector<QPointF> *positions,
                const vector<QColor> *colors,
                QWidget *parent)
        : QWidget(parent), temp(temp), positions(positions), colors(colors){
            QPalette pal = palette();
            pal.setColor(QPalette::Window, Qt::white);
            setAutoFillBackground(true);
            setPalette(pal);
        }

void Canvas::updateDisplay(){
    update();
}

void Canvas::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    if(!temp || !positions || !colors){
        qWarning() << "missing values" ;
        return;
    }
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    float minTemp = std::numeric_limits<float>::max();
    float maxTemp = std::numeric_limits<float>::lowest();

    for (const auto &t : *temp) {
        float val = t.load();
        minTemp = std::min(minTemp, val);
        maxTemp = std::max(maxTemp, val);
    }

    int radius = width()/40;

    for(size_t i=0; i<temp->size(); ++i){
        if(i >= positions->size() || i >= colors->size()){
            qWarning() << "Error";
            continue;
        }

        float currentTemperature = temp->at(i)->load();
        QPointF pos = positions->at(i);
        QColor baseColor = colors->at(i);

        float normalizedTemp = (currentTemperature - minTemp)/(maxTemp - minTemp);
        normalizedTemp = qBound(0.0f, normalizedTemp, 1.0f);

        QColor coldColor(Qt::blue);
        QColor hotColor(Qt::red);
        QColor interpolatedColor;
        interpolatedColor.setRgbF(
            coldColor.redF() * (1.0f - normalizedTemp) + hotColor.redF() * normalizedTemp,
            coldColor.greenF() * (1.0f - normalizedTemp) + hotColor.greenF() * normalizedTemp,
            coldColor.blueF() * (1.0f - normalizedTemp) + hotColor.blueF() * normalizedTemp
        );

        QColor fillColor = interpolatedColor;
        painter.setBrush(fillColor);
        painter.setPen(Qt::black);

        painter.drawEllipse(pos, radius, radius);
        
        QString text = QString::number(currentTemperature, 'f', 1);
        QRectF textRect(pos.x() - radius, pos.y() - radius, radius*2, radius*2);
        painter.setFont(QFont("Arial", 10));
        painter.drawText(textRect, Qt::AlignCenter, text);
    }
}