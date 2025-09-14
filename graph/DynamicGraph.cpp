//
// Created by erick on 14/09/2025.
//

#include "DynamicGraph.h"

#include <random>

DynamicGraph::DynamicGraph()
    : uniformGrid(UniformGrid(0.01)),
      cellSize(0.01),
      minLon(std::numeric_limits<double>::infinity()),
      maxLon(-std::numeric_limits<double>::infinity()),
      minLat(std::numeric_limits<double>::infinity()),
      maxLat(-std::numeric_limits<double>::infinity()) {}

void DynamicGraph::addPoint(const long long id, const double x, const double y) {
    this->idToPoint[id] = Point(id, x, y);
    this->adj[id] = {};

    minLon = std::min(minLon, x);
    maxLon = std::max(maxLon, x);
    minLat = std::min(minLat, y);
    maxLat = std::max(maxLat, y);
}

void DynamicGraph::addEdge(const long long idU, const long long idV, const double dist) {
    Point *pointU = &idToPoint[idU];
    Point *pointV = &idToPoint[idV];

    this->adj[idU].emplace_back(pointU, pointV, dist);
    this->uniformGrid.insertEdge(&this->adj[idU].back());
}

void DynamicGraph::addPolygon(const Polygon &polygon) {
    this->polygons.push_back(polygon);
}

void DynamicGraph::updatePolygonsPosition() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-0.0005, 0.0005);

    for (auto &polygon : this->polygons) {
        bool validMove = false;
        int attempts = 0;

        while (!validMove && attempts < 10) {
            const double dx = dist(gen);
            const double dy = dist(gen);

            validMove = polygon.updatePosition(dx, dy, this->getUniformGrid());
            attempts++;
        }
    }
}
