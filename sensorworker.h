#pragma once

#include <QThread>
#include <atomic>
#include <vector>
#include <QPointF>

using namespace std;

class SensorWorker : public QThread {
    Q_OBJECT
public:
    SensorWorker(int id,
                 vector<shared_ptr<atomic<float>>> *temps,
                 vector<QPointF> *positions,
                 float threshold,
                 QObject *parent = nullptr);

    void stop();

protected:
    void run() override;
private:
    int sensorId;
    vector<shared_ptr<atomic<float>>> *allTemps;
    vector<QPointF> *allPositions;
    float distanceThreshold;
    atomic<bool> running;
};
