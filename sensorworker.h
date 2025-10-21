#pragma once

#include <QThread>
#include <atomic>
#include <vector>
#include <QPointF>

class SensorWorker : public QThread {
    Q_OBJECT
public:
    SensorWorker(int id,
                 std::vector<std::shared_ptr<std::atomic<float>>> *temps,
                 std::vector<QPointF> *positions,
                 float threshold,
                 QObject *parent = nullptr);

    void run() override;

private:
    int id;
    std::vector<std::shared_ptr<std::atomic<float>>> *temps;
    std::vector<QPointF> *positions;
    float threshold;
};
