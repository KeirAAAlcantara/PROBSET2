#include "Canvas.h"
#include <QRectF>
#include <QDebug>
#include <QPoint>

Canvas::Canvas(const vector<shared_ptr<atomic<float>>> *temp,
               const vector<QPointF> *positions,
               const vector<QColor> *colors,
               QWidget *parent)
    : QWidget(parent), temp(temp), positions(positions), colors(colors)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
}

void Canvas::updateDisplay() {
    update();
}

void Canvas::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    if(!temp || !positions || !colors){
        qWarning() << "Missing values";
        return;
    }

    int w = width();
    int h = height();

    QImage img(w, h, QImage::Format_RGB32);

    // Get temperature range
    float minTemp = std::numeric_limits<float>::max();
    float maxTemp = std::numeric_limits<float>::lowest();
    for (const auto &t : *temp) {
        float val = t->load();
        minTemp = std::min(minTemp, val);
        maxTemp = std::max(maxTemp, val);
    }

    // Step 3: Fill each pixel with color of nearest sensor
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int nearestIdx = -1;
            float nearestDist = std::numeric_limits<float>::max();

            // find nearest sensor
            for (size_t i = 0; i < positions->size(); ++i) {
                QPointF p = positions->at(i);
                float dx = x - p.x();
                float dy = y - p.y();
                float dist = dx*dx + dy*dy;
                if (dist < nearestDist) {
                    nearestDist = dist;
                    nearestIdx = i;
                }
            }

            if (nearestIdx >= 0) {
                float currentTemp = temp->at(nearestIdx)->load();
                float normalizedTemp = (currentTemp - minTemp) / (maxTemp - minTemp);
                normalizedTemp = qBound(0.0f, normalizedTemp, 1.0f);

                QColor coldColor(Qt::blue);
                QColor hotColor(Qt::red);
                QColor fillColor;
                fillColor.setRgbF(
                    coldColor.redF() * (1.0f - normalizedTemp) + hotColor.redF() * normalizedTemp,
                    coldColor.greenF() * (1.0f - normalizedTemp) + hotColor.greenF() * normalizedTemp,
                    coldColor.blueF() * (1.0f - normalizedTemp) + hotColor.blueF() * normalizedTemp
                );
                img.setPixelColor(x, y, fillColor);
            }
        }
    }

    // Step 4: Draw image
    QPainter painter(this);
    painter.drawImage(0, 0, img);

    // Step 5: Draw sensor positions + temperature
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10));

    for (size_t i = 0; i < positions->size(); ++i) {
        QPointF pos = positions->at(i);
        float currentTemperature = temp->at(i)->load();
        QString text = QString::number(currentTemperature, 'f', 1);
        painter.drawEllipse(pos, 4, 4);
        painter.drawText(pos + QPointF(6, -6), text);
    }
}
