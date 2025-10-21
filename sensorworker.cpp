#include "sensorworker.h"
#include <QRandomGenerator>
#include <QThread>
#include <cmath>

SensorWorker::SensorWorker(int id,
                           std::vector<std::shared_ptr<std::atomic<float>>> *temps,
                           std::vector<QPointF> *positions,
                           float threshold,
                           QObject *parent)
    : QThread(parent),
      sensorId(id),
      allTemps(temps),
      allPositions(positions),
      distanceThreshold(threshold),
      running(true)
{}

void SensorWorker::stop() {
    running = false;
}

void SensorWorker::run() {
    while (running) {
        if (!allTemps || !allPositions) break;

        float myTemp = (*allTemps)[sensorId]->load();
        QPointF myPos = (*allPositions)[sensorId];

        float sum = myTemp;
        int count = 1;

        for (size_t i = 0; i < allPositions->size(); ++i) {
            if (i == static_cast<size_t>(sensorId)) continue;

            QPointF otherPos = (*allPositions)[i];
            float dx = myPos.x() - otherPos.x();
            float dy = myPos.y() - otherPos.y();
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist <= distanceThreshold) {
                sum += (*allTemps)[i]->load();
                ++count;
            }
        }

        // Smooth convergence
        float avg = sum / count;
        float newTemp = myTemp + 0.1f * (avg - myTemp);
        (*allTemps)[sensorId]->store(newTemp);

        QThread::msleep(QRandomGenerator::global()->bounded(80, 200));
    }
}
