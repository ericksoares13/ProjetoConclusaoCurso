//
// Created by erick on 14/09/2025.
//

#ifndef PROJETOCONCLUSAOCURSO_DYNAMIC_GRAPH_H
#define PROJETOCONCLUSAOCURSO_DYNAMIC_GRAPH_H
#include <list>
#include <unordered_map>
#include <vector>

#include "../geometry/Edge.h"
#include "../geometry/Point.h"
#include "../geometry/Polygon.h"
#include "../geometry/UniformGrid.h"


class Polygon;

class DynamicGraph {
    std::unordered_map<long long, Point> idToPoint;
    std::unordered_map<long long, std::list<Edge>> adj;
    std::vector<Polygon> polygons;
    UniformGrid uniformGrid;
    double cellSize;

    double minLon;
    double maxLon;
    double minLat;
    double maxLat;

    struct DijkstraNode {
        long long id;
        double distance;

        DijkstraNode(const long long id, const double distance) : id(id), distance(distance) {}

        bool operator>(const DijkstraNode& other) const {
            return distance > other.distance;
        }
    };

    struct AStarNode {
        long long id;
        double gCost;
        double fCost;

        AStarNode(const long long id, const double gCost, const double fCost) : id(id), gCost(gCost), fCost(fCost) {}

        bool operator>(const AStarNode& other) const {
            return fCost > other.fCost;
        }
    };

public:
    DynamicGraph();

    void addPoint(long long id, double x, double y);
    void addEdge(long long idU, long long idV, double dist);
    void addPolygon(const Polygon &polygon);

    void updatePolygonsPosition();
    std::vector<long long> findPathDijkstra(long long idU, long long idV);
    long long nextPointConsideringPolygonsDijkstra(long long idU, long long idV);
    std::vector<long long> findPathAStar(long long idU, long long idV);
    long long nextPointConsideringPolygonsAStar(long long idU, long long idV);

    const std::unordered_map<long long, Point> &getIdToPoint() const { return this->idToPoint; }
    const std::unordered_map<long long, std::list<Edge>> &getAdj() const { return this->adj; }
    const std::vector<Polygon> &getPolygons() const { return this->polygons; }
    const UniformGrid &getUniformGrid() const { return this->uniformGrid; }
    double getCellSize() const { return this->cellSize; }
    double getMinLon() const { return this->minLon; }
    double getMaxLon() const { return this->maxLon; }
    double getMinLat() const { return this->minLat; }
    double getMaxLat() const { return this->maxLat; }
};


#endif //PROJETOCONCLUSAOCURSO_DYNAMIC_GRAPH_H