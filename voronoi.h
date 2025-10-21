
#pragma once
#include <vector>
#include <atomic>
#include <thread>
#include <cmath> 
#include <QPoint> 

using namespace std;

class voronoi {

public: 
    voronoi(int id, float threshold, vector<atomic<float>> *temp, 
            vector<QPointF> *positions);

    void start();
    void stop();

    static atomic<bool> running;

private:
    int id; 
    float threshold;
    vector<atomic<float>> *temp;
    vector<QPointF> *positions;
    thread workerThread;

    void run();
    bool isNeighbor(int other Id);
};