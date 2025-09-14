//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_DYNAMIC_GRAPH_H
#define PROJETOCONCLUSAOCURSO_DYNAMIC_GRAPH_H
#include <unordered_map>
#include <vector>

#include "../geometry/Edge.h"
#include "../geometry/Point.h"
#include "../geometry/UniformGrid.h"


class DynamicGraph {
    std::unordered_map<long long, Point> idToPoint;
    std::unordered_map<long long, std::vector<Edge>> adj;
    UniformGrid uniformGrid;
    double cellSize;

    double minLon;
    double maxLon;
    double minLat;
    double maxLat;

public:
    DynamicGraph();

    void addPoint(long long id, double x, double y);
    void addEdge(long long idU, long long idV, double dist);

    std::unordered_map<long long, Point> getIdToPoint() const { return this->idToPoint; }
    std::unordered_map<long long, std::vector<Edge>> getAdj() const { return this->adj; }
    UniformGrid &getUniformGrid() { return this->uniformGrid; }
    double getCellSize() const { return this->cellSize; }
    double getMinLon() const { return this->minLon; }
    double getMaxLon() const { return this->maxLon; }
    double getMinLat() const { return this->minLat; }
    double getMaxLat() const { return this->maxLat; }
};


#endif //PROJETOCONCLUSAOCURSO_DYNAMIC_GRAPH_H