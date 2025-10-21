#include "voronoi.h"
#include <chrono>

atomic<bool> voronoi::running{true};

voronoi::voronoi(int id, float threshold, vaector<atomic<float>> *temp, 
                vector<QPointF> *positions)
    : id(id), threshold(threshold), temp(temp), positions(positions){}

void voronoi::start(){
    workerThread = thread(&voronoi::run, this);
}

void voronoi::stop(){
    running = false;
    if(workerThread.joinable()) workerThread.join();
}

bool voronoi::isNeighbor(int otherId){
    QPointF p1 = positions->at(id);
    QPointF p2 = positions->at(otherId);
    float dx = p1.x() - p2.x();
    float dy = p1.y() - p2.y();

    return sqrt(dx*dx + dy*dy) <=threshold;
}

void voronoi::run(){
    while(running){
        float sum = temp->at(id).load();
        int count = 1; 

        for(size_t j=0; j<temp->size(); ++j){
            if(j==id) continue'
            if(isNeighbor(j)){
                sum+=temp->at(j).load();
                count++;
            }
        }
        float avg=sum/count;
        temp->at(id).store(avg);

        this_thread::sleep_for(chrono::milliseconds(100));
    }
}