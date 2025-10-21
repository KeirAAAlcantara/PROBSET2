
#pragma once
#include <vector>
#include <atomic>
#include <thread>
#include <QPointF> 
#include <cmath> 

using namespace std;

class voronoi {

public: 
    voronoi(int id, float threshold, vector<atomic<float>> *allTemp, 
            const vector<QPointF> *allPositions);

    void start();
    void stop();
    bool isRunning() const {
        return running.load();
    }

private:
    void run();
    bool isNeighbor(int otherId);

    int id; 
    float threshold;

    vector<atomic<float>> *allTemp;
    const vector<QPointF> *allPositions;

    thread workerThread;
    atomic<bool> running;
};