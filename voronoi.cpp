#include "voronoi.h"
#include <chrono>
#include <QDebug>

voronoi::voronoi(int id, float threshold, vector<atomic<float>> *allTemp, 
                const vector<QPointF> *allPositions)
    : id(id), threshold(threshold), allTemp(allTemp), allPositions(allPositions), running(false) {}

voronoi::~voronoi() {
    stop();
}

void voronoi::start() {
    if (!running.load()) {
        running = true;
        workerThread = thread(&voronoi::run, this);
        qInfo() << "Voronoi thread" << id << "started";
    }
}

void voronoi::stop() {
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
        qInfo() << "Voronoi thread" << id << "stopped";
    }
}

bool voronoi::isNeighbor(int otherId) {
    if (id < 0 || id >= allPositions->size() || otherId < 0 || otherId >= allPositions->size())
        return false;

    QPointF p1 = allPositions->at(id);
    QPointF p2 = allPositions->at(otherId);
    float dx = p1.x() - p2.x();
    float dy = p1.y() - p2.y();

    return sqrt(dx * dx + dy * dy) <= threshold;
}

void voronoi::run() {
    while (running.load()) {
        float sum = allTemp->at(id)->load();
        int count = 1;

        for (size_t j = 0; j < allTemp->size(); ++j) {
            if (j == id) continue;
            if (isNeighbor(j)) {
                sum += allTemp->at(j)->load();
                count++;
            }
        }

        float avg = sum / count;
        allTemp->at(id)->store(avg);

        this_thread::sleep_for(chrono::milliseconds(100));
    }
}
