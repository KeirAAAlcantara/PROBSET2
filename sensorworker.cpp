#include "sensorworker.h"
#include <QRandomGenerator>
#include <QThread>
#include <cmath>

//using namespace std;

SensorWorker::SensorWorker(int id,
                           std::vector<std::shared_ptr<std::atomic<float>>> *temps,
                           std::vector<QPointF> *positions,
                           float threshold,
                           QObject *parent)
    : QThread(parent), id(id), temps(temps), positions(positions), threshold(threshold) {}

void SensorWorker::run() {
    while (true) {
        float myTemp = temps->at(id)->load();
        QPointF myPos = positions->at(id);

        // Read nearby sensors
        float sum = 0;
        int count = 0;

        for (size_t i = 0; i < positions->size(); ++i) {
            if (i == (size_t)id) continue;

            QPointF otherPos = positions->at(i);
            float dist = std::hypot(myPos.x() - otherPos.x(), myPos.y() - otherPos.y());
            if (dist < threshold) {
                sum += temps->at(i)->load();
                ++count;
            }
        }

        // Update my temperature based on neighbors
        if (count > 0) {
            float avg = sum / count;
            float newTemp = myTemp + 0.1f * (avg - myTemp);  // small convergence factor
            temps->at(id)->store(newTemp);
        }

        QThread::msleep(QRandomGenerator::global()->bounded(50, 200));
    }
}
